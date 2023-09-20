#include <stdio.h>
#include <curl/curl.h>

#define URL "https://mirrors.mit.edu/archlinux/iso/latest/"

size_t curl_output(char* ptr, size_t size, size_t nmemb, void* userdata) {
    
}

int main(void) {
    CURL* handle;
    handle = curl_easy_init(); // CURL easy handle
    if (handle) {
        CURLcode response;
        curl_easy_setopt(handle, CURLOPT_URL, URL);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_output);
        response = curl_easy_perform(handle);
        curl_easy_cleanup(handle);

        //printf("%d\n", response);
    }
    else fprintf(stderr, "Error!\n");
    return 0;
}
