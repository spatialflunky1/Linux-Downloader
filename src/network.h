#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define ARCH_URL "https://mirrors.mit.edu/archlinux/iso/"
#define GENTOO_URL "https://mirrors.mit.edu/gentoo-distfiles/releases/"
#define UBUNTU_URL "https://mirrors.mit.edu/ubuntu-releases/"

#define MEMORY_ERROR "Unable to allocate memory\n"
#define NETWORK_ERROR "Unknown Network Error\n"
#define CURL_ERROR "Curl Library Errror\n"

typedef struct memory {
    char* html_body;
    size_t html_size;
} memory;

size_t write_callback_html(char* ptr, size_t size, size_t nmemb, memory* mem);
size_t write_data(void* data, size_t size, size_t nmemb, FILE* stream);
int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
void append_string(char c, char** string, int* len);
void append_string_string(char* s, char** string, int* len);
void append_string_array(char* s, char*** array, int* len);
void get_files(char* URL, char*** files, int* files_len, int distro);
void get_directories(char* URL, char*** dirs, int* dirs_len, int special_modes);
void download_file(int distro, char* URL_base, char* filename);
