#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <ncurses.h>
#include <iostream>
#include <curl/curl.h>

// Global Vars
bool running=true;
bool need_update=true;
int key; // Key id
int current=0; // Current menu
int current_selection=0; // Selected item
int selected; // Selection from last menu
int bottom; // Bottom location (length of menu list)
std::vector<std::string> menu;

// Constants
const std::string url = "72.231.177.233"; // IP of my server
const int port = 80; // Port of my server

void cleanup(WINDOW * mainWindow) {
    delwin(mainWindow);
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}

void update_selection(WINDOW * mainWindow) {
    key = getch();
    refresh();
    switch (key) {
        case 66:         
            if (current_selection<bottom-1) current_selection++;
            key=0;
            break;
        case 65:
            if (current_selection>0) current_selection--;
            key=0;
            break;
        case 10:
            if (current < 3) current++;
            selected=current_selection;
            if (current==3) running=false;
            else {
                current_selection=0;
                need_update=true;
            }
            clear();
            break;
        case 113:
            running=false;
            cleanup(mainWindow);
            exit(0);
            break;
    }
}

void dialog(std::vector<std::string> menu, int length, std::string title) {
    attron(COLOR_PAIR(3));
    mvprintw(0,0,menu.at(0).c_str());
    for (int i=1; i<length; i++) {
        if (current_selection+1==i) attron(COLOR_PAIR(2));
        else attron(COLOR_PAIR(1));
        mvprintw(i+1,0,menu.at(i).c_str());
    }
    refresh();
}

std::vector<std::string> get_data(std::string distro, std::string request,std::string title) {
    std::vector<std::string> versions;
    httplib::Client cli(url,port);
    // Request ubuntu versions from the server
    auto res = cli.Post("/", request, "text/plain");
    // If data gets returned
    if (res) {
        std::string body_text = res->body;
        versions.push_back(title);
        std::stringstream s_stream(body_text);
        while (s_stream.good()) {
            std::string substr;
            getline(s_stream,substr,',');
            versions.push_back(substr);
        }
    }
    // If no data is returned print the error
    else {
        std::string message = (std::string)"Error: "+httplib::to_string(res.error())+(std::string)"\n";
        endwin();
        std::cout << message << std::endl;
        exit(0);
    }
    return versions;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr,size,nmemb, (FILE *)stream);
    return written;
}

void download_file(std::string downUrl, std::string filename) {
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

int main() {
    std::vector<std::string> files;
    std::vector<std::string> links;
    std::string distro;
    std::string version;
    WINDOW * mainWindow;
    if ((mainWindow = initscr()) == NULL) {
        std::cout << "Failed to start ncurses" << std::endl;
        exit(1);
    } // start curses mode or exit on error
    start_color();
    use_default_colors();
    init_pair(1,COLOR_WHITE,-1); // 1: No highlight
    init_pair(2,COLOR_BLACK,COLOR_WHITE); // 2: highlight
    init_pair(3,COLOR_RED,COLOR_CYAN); // 3: title highlight
    noecho();
    curs_set(0);
    clear();
    while (running) {
        if (current==0) {
            if (need_update) {
                menu=get_data("","menu","Select a Distro:");
                bottom=menu.size()-1;
                need_update=false;
            }
            dialog(menu,menu.size(),distro); 
        }
        if (current==1) {
            if (need_update) {
                clear();
                mvprintw(0,0,(char*)"Loading Versions...");
                refresh();
                distro=menu.at(selected+1);
                menu = get_data(distro, distro+" getvers","Select Version:");
                bottom=menu.size()-1;
                need_update = false;
                clear();
                refresh();
            }
            dialog(menu,menu.size(),distro);
        }
        if (current==2) {
            if (need_update) {
                clear();
                mvprintw(0,0,(char*)"Loading Files...");
                refresh();
                version=menu.at(selected+1);
                std::vector<std::string> filesWLinks = get_data(distro,distro+" getfiles "+version,"Select File:");
                for (int i=0; i<filesWLinks.size(); i++) {
                    if (i%2==1 || i==0) files.push_back(filesWLinks.at(i));
                    else links.push_back(filesWLinks.at(i));
                }
                bottom=files.size()-1;
                need_update=false;
                clear();
                refresh();
            }
            dialog(files,files.size(),distro);
        }
        update_selection(mainWindow);
        refresh();
    }
    cleanup(mainWindow);
    download_file(links.at(current_selection), files.at(current_selection+1));
    return 0;
}
