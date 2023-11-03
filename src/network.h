#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define ARCH_URL "https://mirrors.mit.edu/archlinux/iso/latest/"
#define GENTOO_URL "https://distfiles.gentoo.org/releases/"

typedef struct memory {
    char* html_body;
    size_t html_size;
} memory;

size_t write_callback_html(char* ptr, size_t size, size_t nmemb, memory* mem);
size_t write_data(void* data, size_t size, size_t nmemb, FILE* stream);
int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
void append_string(char c, char** string, int* len);
void append_string_array(char* s, char*** array, int* len);
void get_gentoo_versions(char*** versions, int* vers_len, memory* mem);
void get_versions(int distro, char*** versions, int* vers_len, char* arch);
void get_gentoo_archs(char*** archs, int* archs_len, memory* mem);
void get_archs(int distro, char*** archs, int* archs_len);
void get_arch_files(char*** files, int* files_len, memory* mem);
void get_archs(int distro, char*** archs, int* archs_len);
void get_files(int distro, char*** files, int* files_len, char* arch, char* version);
void download_file(int distro, char* filename);
