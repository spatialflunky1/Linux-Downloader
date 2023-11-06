#include "network.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/system.h>
#include <stdio.h>

size_t write_callback_html(char* data, size_t size, size_t nmemb, memory* mem) {
    size_t realsize = size * nmemb;
    char* ptr = realloc(mem->html_body, mem->html_size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, MEMORY_ERROR);
        exit(1);
    }
    mem->html_body = ptr;
    memcpy(&(mem->html_body[mem->html_size]), data, realsize);
    mem->html_size += realsize;
    mem->html_body[mem->html_size] = 0;
    return realsize;
}

size_t write_data(void* data, size_t size, size_t nmemb, FILE* stream) {
    size_t filesize = fwrite(data, size, nmemb, stream);
    return filesize;
}

int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal <= 0.0) {
        return 0;
    }
    printf("\x1b[d"); // return to home position
    printf("Downloading... %ld%%\n", dlnow*100/dltotal);
    fflush(stdout);
    return 0;
}

void append_string(char c, char** string, int* len) { 
    *string = realloc(*string, ((*len) + 1) * sizeof(char));
    if (*string == NULL) {
        fprintf(stderr, MEMORY_ERROR);
        exit(1);
    }
    (*string)[*len] = c;
    (*len)++;
}

void append_string_string(char* s, char** string, int* len) {
    *string = realloc(*string, ((*len) + strlen(s) + 1) * sizeof(char));
    if (*string == NULL) {
        fprintf(stderr, MEMORY_ERROR);
        exit(1);
    }
    if ((*len) == 0) {
        strcpy(*string, s);
    }
    else {
        strcat(*string, s);
    }
    (*len) += strlen(s);
}

void append_string_array(char* s, char*** array, int* len) {
    *array = realloc(*array, ((*len) + 1) * sizeof(char*));
    if (*array == NULL) {
        fprintf(stderr, MEMORY_ERROR);
        exit(1);
    }
    (*array)[*len] = s;
    (*len)++;
}

memory* get_html(char* URL) {
    CURL* handle;
    handle = curl_easy_init();
    memory* mem = malloc(sizeof(memory));
    if (mem == NULL) {
        fprintf(stderr, MEMORY_ERROR);
        exit(1);
    }
    mem->html_body = NULL;
    mem->html_size = 0;
    if (handle) {
        CURLcode response;
        curl_easy_setopt(handle, CURLOPT_URL, URL);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback_html);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, mem);
        response = curl_easy_perform(handle);
        if (response != 0) {
            fprintf(stderr, NETWORK_ERROR);
            exit(1);
        }
        curl_easy_cleanup(handle);
    }
    else {
        fprintf(stderr, CURL_ERROR);
        exit(1);
    }
    return mem;
}

void get_gentoo_versions(char*** versions, int* vers_len, memory* mem) {
    int in_line = 0;
    char* version = NULL;
    int ver_len = 0;
    for (int i = 0; i < mem->html_size; i++) {
        if (    mem->html_body[i] == '[' &&
                i < mem->html_size - 4 &&
                mem->html_body[i+1] == 'D' &&
                mem->html_body[i+2] == 'I' &&
                mem->html_body[i+3] == 'R' &&
                mem->html_body[i+4] == ']') {
            in_line = 1;
            i+=5;
        }
        else if (mem->html_body[i] == '[' &&
                 i < mem->html_size - 4 &&
                 mem->html_body[i+1] == ' ' &&
                 mem->html_body[i+2] == ' ' &&
                 mem->html_body[i+3] == ' ' &&
                 mem->html_body[i+4] == ']') {
            // Stop checking after readme (first file) is detected0
            break;
        }
        else if (in_line) {
            if (mem->html_body[i] == '"') {
                while (i++, mem->html_body[i] != '/') append_string(mem->html_body[i], &version, &ver_len);
                append_string('\0', &version, &ver_len);
                append_string_array(version, versions, vers_len);
                version = NULL;
                ver_len = 0;
                in_line = 0;
            }
        }
   }
}

void get_versions(int distro, char*** versions, int* vers_len, char* URL) {
    memory* mem = NULL;
    switch (distro) {
        case 2: {
            mem = get_html(URL);
            get_gentoo_versions(versions, vers_len, mem);
            break;
        }
    }
    if (mem != NULL && mem->html_body != NULL) {
        free(mem->html_body);
        free(mem);    
    }
}

void get_gentoo_archs(char*** archs, int* archs_len, memory* mem) {
    int in_line = 0;
    int num_close = 0;
    char* arch = NULL;
    int arch_len = 0;
    for (int i = 0; i < mem->html_size; i++) {
        if (    mem->html_body[i] == '[' && 
                i < mem->html_size - 4 && 
                mem->html_body[i+1] == 'D' && 
                mem->html_body[i+2] == 'I' && 
                mem->html_body[i+3] == 'R' && 
                mem->html_body[i+4] == ']') {
            in_line = 1;
        }
        else if (in_line) {
            if (mem->html_body[i] == '>') {
                num_close++;
                i++;
            }
            else if (num_close == 4 && mem->html_body[i] == '/') {
                in_line = 0;
                num_close = 0;
                append_string('\0', &arch, &arch_len);
                append_string_array(arch, archs, archs_len);
                arch = NULL;
                arch_len = 0;
            }
        }

        if (num_close == 4) {
            append_string(mem->html_body[i], &arch, &arch_len);
        }
   }
}

void get_archs(int distro, char*** archs, int* archs_len) {
    memory* mem = NULL;
    switch (distro) {
        case 2: {
            mem = get_html(GENTOO_URL);
            get_gentoo_archs(archs, archs_len, mem);
            break;
        }
    }
    if (mem != NULL && mem->html_body != NULL) {
        free(mem->html_body);
        free(mem);    
    }
}

void get_arch_files(char*** files, int* files_len, memory* mem) {
    int inside = 0;
    char* tmp = NULL;
    int tmp_len = 0;
    for (int i = 0; i < mem->html_size; i++) {
        if (mem->html_body[i] == '"') {
            if (inside == 1) {
                append_string('\0', &tmp, &tmp_len);
                if (strstr(tmp, "/") == NULL) {
                    append_string_array(tmp, files, files_len);
                }
                tmp = NULL;
                tmp_len = 0;
                inside = 0;
            }
            else {
                i++;
                inside = 1;
            }
        }
        if (inside) {
            append_string(mem->html_body[i], &tmp, &tmp_len);
        }
    }
}

void get_gentoo_files(char*** files, int* files_len, memory* mem) {
    int in_line = 0;
    char* tmp = NULL;
    int tmp_len = 0;
    for (int i = 0; i < mem->html_size; i++) {
        if (    mem->html_body[i] == '[' &&
                i < mem->html_size - 4 &&
                mem->html_body[i+1] == ' ' &&
                mem->html_body[i+2] == ' ' &&
                mem->html_body[i+3] == ' ' &&
                mem->html_body[i+4] == ']') {
            in_line = 1;
            i+=6;
        }
        if (in_line) {
            if (mem->html_body[i] == '"') {
                while(i++, mem->html_body[i] != '"') append_string(mem->html_body[i], &tmp, &tmp_len);
                append_string('\0', &tmp, &tmp_len);
                if (!strstr(tmp, "CONTENTS") && (tmp[tmp_len-2] == 'o' || tmp[tmp_len-2] == 'z')) {
                    append_string_array(tmp, files, files_len);
                }
                in_line = 0;
                tmp = NULL;
                tmp_len = 0;
            }
        }
    }
}

void get_files(int distro, char*** files, int* files_len, char* URL) {
    memory* mem = NULL;
    mem = get_html(URL);
    switch (distro) {
        case 1: {
            get_arch_files(files, files_len, mem);
            break;
        }
        case 2: { 
            get_gentoo_files(files, files_len, mem);
            break;
        }
    }
    if (mem != NULL && mem->html_body != NULL) {
        free(mem->html_body);
        free(mem);
    }
}

void download_file(int distro, char* URL_base, char* filename) {
    char* URL = malloc((strlen(URL_base) + strlen(filename) + 1) * sizeof(char));
    if (URL == NULL) {
        fprintf(stderr, MEMORY_ERROR);
        exit(1);
    }
    strcpy(URL, URL_base);
    strcat(URL, filename);
    
    CURL* handle;
    handle = curl_easy_init();
    if (handle) {
        CURLcode response;
        FILE* fptr = fopen(filename, "wb");
        curl_easy_setopt(handle, CURLOPT_URL, URL);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, fptr);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0);
        response = curl_easy_perform(handle);
        if (response != 0) {
            fprintf(stderr, NETWORK_ERROR);
            exit(1);
        }

        curl_easy_cleanup(handle);
        fclose(fptr);
    }
    else {
        fprintf(stderr, CURL_ERROR);
        exit(1);
    }

    free(URL);
}
