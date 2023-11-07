#include "network.h"
#include <ctype.h>
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

/* Special Modes:
 * 0: none
 * 1: Gentoo Files
 */
void get_files(char* URL, char*** files, int* files_len, int special_modes) {
    memory* mem = get_html(URL);
    int inside = 0;
    char* tmp = NULL;
    int tmp_len = 0;
    for (int i = 0; i < mem->html_size; i++) {
        if (mem->html_body[i] == '"') {
            if (inside == 1) {
                append_string('\0', &tmp, &tmp_len);
                if (strstr(tmp, "/") == NULL) {
                    if (special_modes == 1) {
                        if (strstr(tmp, "CONTENTS") == NULL && (tmp[tmp_len-2]=='o' || tmp[tmp_len-2]=='z')) {
                            append_string_array(tmp, files, files_len);
                        }
                    }
                    else {
                        append_string_array(tmp, files, files_len);
                    }
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

/* Special Modes:
 * 0: none
 * 1: Gentoo Versions
 */
void get_directories(char* URL, char*** dirs, int* dirs_len, int special_modes) {
    memory* mem = get_html(URL);
    int in_line = 0;
    char* tmp = NULL;
    int tmp_len = 0;
    for (int i = 0; i < mem->html_size; i++) {
        if (mem->html_body[i] == '"' && in_line == 0) {
            while (i++, mem->html_body[i] != '/') {
                if (mem->html_body[i] == '"') {
                    tmp = NULL;
                    tmp_len = 0;
                    break;
                }
                append_string(mem->html_body[i], &tmp, &tmp_len);
            }
            if (tmp != NULL) {
                append_string('\0', &tmp, &tmp_len);
                if (tmp[0] != '.') {
                    if (special_modes == 1 && !isdigit(tmp[0])) {
                        return;
                    }
                    append_string_array(tmp, dirs, dirs_len);
                }
                tmp = NULL;
                tmp_len = 0;
                in_line = 1;
            }
        }
        if (mem->html_body[i] == '\n') {
            in_line = 0;
        }
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
