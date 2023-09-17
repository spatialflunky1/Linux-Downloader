#include "network.h"

// Constants
const std::string url = "72.231.177.233"; // IP of my server
const int port = 80; // Port of my server

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

std::vector<std::string> get_data(std::string distro, std::string request,std::string title, WINDOW * mainWindow) {
    std::vector<std::string> menu;
    httplib::Client cli(url,port);
    auto res = cli.Post("/", request, "text/plain");
    // If data gets returned
    if (res) {
        std::string body_text = res->body;
        if (distro.compare("Linux Kernel")==0) menu.push_back("https://mirrors.edge.kernel.org/");
        menu.push_back(title);
        std::stringstream s_stream(body_text);
        while (s_stream.good()) {
            std::string substr;
            getline(s_stream,substr,',');
            menu.push_back(substr);
        }
    }
    // If no data is returned print the error
    else {
        std::string message = (std::string)"Error: "+httplib::to_string(res.error())+(std::string)"\n";
        cleanup(mainWindow);
        std::cout << message << std::endl;
        exit(0);
    }
    return menu;
}
