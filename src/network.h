#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define ARCH_URL "https://mirrors.mit.edu/archlinux/iso/latest/"

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
void append_string(char c, char** string, int* len);
void append_string_array(char* s, char*** array, int* len);
void get_arch_files(char*** files, int* files_len);
void get_files(int distro, char*** files, int* files_len);
void download_file(int distro, char* filename);
