#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define URL "https://mirrors.mit.edu/archlinux/iso/latest/"

char* html_body = NULL;
size_t html_size;

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    html_body = malloc((nmemb + 1) * sizeof(char));
    strcpy(html_body, ptr);
    html_size = nmemb; 
}

// Find reason for segfault
void append_string(char c, char** string, int* len) {
    char* new = malloc(((*len) + 1) * sizeof(char));
    if (new == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    strcpy(new, *string);
    if (*string != NULL) free(*string);
    new[(*len)] = c;
    (*len)++;
    *string = new;
}

void append_string_array(char* s, char*** array, int* len) {

}

int main(void) {
    char** files = NULL;
    CURL* handle;
    handle = curl_easy_init(); // CURL easy handle
    if (handle) {
        CURLcode response;
        curl_easy_setopt(handle, CURLOPT_URL, URL);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
        response = curl_easy_perform(handle);
        curl_easy_cleanup(handle);

        int inside = 0;
        char* tmp = NULL;
        int  tmp_len = 1; // set to one so null byte inside after first append
        for (int i = 0; i < html_size; i++) {
            if (html_body[i] == '"') {
                if (inside == 1) {
                    putchar('\n');
                    inside = 0;
                }
                else {
                    i++;
                    inside = 1;
                }
            }
            if (inside) 
                append_string(html_body[i], &tmp, &tmp_len);
        }
        
        putchar('\n');

        if (html_body != NULL) free(html_body);
    }
    else fprintf(stderr, "Error!\n");
    return 0;
}
