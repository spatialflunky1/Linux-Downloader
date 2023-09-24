#include "network.h"

char* html_body = NULL;
size_t html_size;

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    html_body = malloc((nmemb + 1) * sizeof(char));
    strcpy(html_body, ptr);
    html_size = nmemb; 
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

void get_arch_files(char*** files, int* files_len) {
    int inside = 0;
    char* tmp = NULL;
    int  tmp_len = 0; // set to one so null byte inside after first append
    for (int i = 0; i < html_size; i++) {
        if (html_body[i] == '"') {
            if (inside == 1) {
                append_string('\0', &tmp, &tmp_len);
                append_string_array(tmp, files, files_len);
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
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
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
