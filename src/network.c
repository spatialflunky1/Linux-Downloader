#include "network.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/system.h>
#include <stdio.h>

char* html_body = NULL;
size_t html_size;

size_t write_callback_html(char* ptr, size_t size, size_t nmemb, void* userdata) {
    html_body = malloc((nmemb + 1) * sizeof(char));
    strcpy(html_body, ptr);
    html_size = nmemb;
    return size * nmemb;
}

size_t write_data(void* data, size_t size, size_t nmemb, FILE* stream) {
    size_t filesize = fwrite(data, size, nmemb, stream);
    return filesize;
}

int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal <= 0.0) return 0;
    printf("\x1b[d"); // return to home position
    printf("Downloading... %ld%%\n", dlnow*100/dltotal);
    fflush(stdout);
    return 0;
}

void append_string(char c, char** string, int* len) { 
    *string = realloc(*string, ((*len) + 1) * sizeof(char));
    if (*string == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    (*string)[*len] = c;
    (*len)++;
}

void append_string_array(char* s, char*** array, int* len) {
    *array = realloc(*array, ((*len) + 1) * sizeof(char*));
    if (*array == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    (*array)[*len] = s;
    (*len)++;
}

void get_gentoo_archs(char*** archs, int* archs_len) {
    int in_line = 0;
    int num_close = 0;
    char* arch = NULL;
    int arch_len = 0;
    for (int i = 0; i < html_size; i++) {
        if (html_body[i] == '[' && i < html_size - 4 && html_body[i+1] == 'D' && html_body[i+2] == 'I' && html_body[i+3] == 'R' && html_body[i+4] == ']') {
            in_line = 1;
        }
        else if (in_line){
            if (html_body[i] == '>') {
                num_close++;
                i++;
            }
            else if (num_close == 4 && html_body[i] == '/') {
                in_line = 0;
                num_close = 0;
                append_string('\0', &arch, &arch_len);
                append_string_array(arch, archs, archs_len);
                arch = NULL;
                arch_len = 0;
            }
        }

        if (num_close == 4) append_string(html_body[i], &arch, &arch_len);
   }
}

void get_archs(int distro, char ***archs, int* archs_len) {
    CURL* handle;
    handle = curl_easy_init();
    if (handle) {
        CURLcode response;
        switch (distro) {
            case 2:
                curl_easy_setopt(handle, CURLOPT_URL, GENTOO_ARCHS_URL);
                break;
        }
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback_html);
        response = curl_easy_perform(handle);
        if (response != 0) {
            fprintf(stderr, "Network Error!\n");
            exit(1);
        }
        curl_easy_cleanup(handle);

        switch (distro) {
            case 2:
                get_gentoo_archs(archs, archs_len);
                break;
        }

        if (html_body != NULL) free(html_body);
    }
    else {
        fprintf(stderr, "Curl Error!\n");
        exit(1);
    }
}

void get_arch_files(char*** files, int* files_len) {
    int inside = 0;
    char* tmp = NULL;
    int  tmp_len = 0; // set to one so null byte inside after first append
    for (int i = 0; i < html_size; i++) {
        if (html_body[i] == '"') {
            if (inside == 1) {
                append_string('\0', &tmp, &tmp_len);
                if (strstr(tmp, "/") == NULL) append_string_array(tmp, files, files_len);
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
            append_string(html_body[i], &tmp, &tmp_len);
        }
    }
}

void get_files(int distro, char*** files, int* files_len) {
    CURL* handle;
    handle = curl_easy_init(); // CURL easy handle
    if (handle) {
        CURLcode response;
        switch (distro) {
            case 1:
                curl_easy_setopt(handle, CURLOPT_URL, ARCH_URL);
                break;
        }
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback_html);
        response = curl_easy_perform(handle);
        if (response != 0) {
            fprintf(stderr, "Network Error!\n");
            exit(1);
        }
        curl_easy_cleanup(handle);

        // Uses the global html_body and html_size, so must be set
        switch (distro) {
            case 1:
                get_arch_files(files, files_len);
                break;
        }

        // html_body is no longer needed
        if (html_body != NULL) free(html_body);
    }
    else {
        fprintf(stderr, "Curl Error!\n");
        exit(1);
    }
}

void download_file(int distro, char* filename) {
    char* URL = NULL;
    switch (distro) {
        case 1:
            URL = malloc((strlen(ARCH_URL) + strlen(filename) + 1) * sizeof(char));
            if (URL == NULL) {
                fprintf(stderr, "Unable to allocate memory\n");
                exit(1);
            }
            strcpy(URL, ARCH_URL);
            strcat(URL, filename);
            break;
    }
    if (URL == NULL) {
        fprintf(stderr, "Error!\n");
        exit(1);
    }
    
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
            fprintf(stderr, "Network Error\n");
            exit(1);
        }

        curl_easy_cleanup(handle);
        fclose(fptr);
    }
    else {
        fprintf(stderr, "Curl Error!\n");
        exit(1);
    }

    free(URL);
}
