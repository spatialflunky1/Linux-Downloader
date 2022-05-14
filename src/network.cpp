#include "network.h"
#include <curl/curl.h>
#include <iostream>
#include <cstring>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr,size,nmemb, (FILE *)stream);
    return written;
}

void download_file(std::string downUrl, std::string filename) {
    std::cout << downUrl << "\n";
    std::cout << filename << "\n";
    // vars
    CURL *easyhandle;    
    const char *pagefilename = filename.c_str();
    FILE *pagefile;
    // Curl setup
    curl_global_init(CURL_GLOBAL_ALL);
    easyhandle = curl_easy_init();
    // curl_easy_setopt(easyhandle, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(easyhandle, CURLOPT_URL, downUrl.c_str());
    curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easyhandle, CURLOPT_NOPROGRESS, 0);
    // Get path
    std::string path;
    std::cout << "Enter path to download to: ";
    std::getline(std::cin, path);
    // Download file
    pagefile = fopen(pagefilename, "wb");
    if (pagefile) {
        curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, pagefile);
        curl_easy_perform(easyhandle);
        fclose(pagefile);
    }
    // Cleanup
    curl_easy_cleanup(easyhandle);
    curl_global_cleanup();
    if (std::rename(filename.c_str(), (path+"/"+filename).c_str())<0) {
        std::cout << strerror(errno) << "\n";
    }
}
