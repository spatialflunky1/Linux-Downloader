#include <cstddef>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include "window.h"
#include "httplib.h"

// Write downloaded data to file
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
// Download data from curl libs
void download_file(std::string downUrl, std::string filename);
// Gets data from the server
std::vector<std::string> get_data(std::string distro, std::string request,std::string title, WINDOW * mainWindow);
