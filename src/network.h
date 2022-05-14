#include <cstddef>
#include <iostream>

// Write downloaded data to file
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
// Download data from curl libs
void download_file(std::string downUrl, std::string filename);
