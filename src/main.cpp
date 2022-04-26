#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <ncurses.h>
#include <iostream>

// Global Vars
int key; // Key id
bool running=true;
int current=0; // Current menu
int current_selection=0; // Selected item
int selected; // Selection from last menu
int bottom; // Bottom location (length of menu list)

// Constants
const std::string url = "72.231.177.233"; // IP of my server
const int port = 80; // Port of my server
const std::vector<std::string> distros = {"Select a distro:","Ubuntu","Linux Kernel"}; // Menu 1
const int length1=distros.size();
const std::vector<std::string> states = {"distro","version","files"}; // Possible states of program

void update_selection() {
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
            if (current < 2) current++;
            selected=current_selection;
            current_selection=0;
            clear();
            break;
        case 113:
            running=false;
            break;
    }
}

void dialog(std::vector<std::string> menu, int length, std::string title) {
    attron(COLOR_PAIR(3));
    mvprintw(0,0,menu[0].c_str());
    for (int i=1; i<length; i++) {
        if (current_selection+1==i) attron(COLOR_PAIR(2));
        else attron(COLOR_PAIR(1));
        mvprintw(i+1,0,menu[i].c_str());
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
        if (distro.compare((std::string)"Ubuntu")==0) {
            versions.push_back(title);
            std::stringstream s_stream(body_text);
            while (s_stream.good()) {
                std::string substr;
                getline(s_stream,substr,',');
                versions.push_back(substr);
            }
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

int main() {
    bottom=length1-1;
    std::vector<std::string> versions;
    std::vector<std::string> files;
    bool need_versions=true;
    bool need_files=true;
    std::string distro;
    std::string version;
    WINDOW * mainWindow;
    if ((mainWindow = initscr()) == NULL) {
        std::cout << "Failed to start ncurses" << std::endl;
        exit(EXIT_FAILURE);
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
        if (states.at(current).compare("distro")==0) {
            dialog(distros,length1,distro); 
        }
        if (states.at(current).compare("version")==0) {
            if (need_versions) {
                clear();
                mvprintw(0,0,(char*)"Loading Versions...");
                refresh();
                distro=distros.at(selected+1);
                versions = get_data(distro, distro+" getvers","Select version");
                bottom=versions.size()-1;
                need_versions = false;
                clear();
                refresh();
            }
            dialog(versions,versions.size(),distro);
        }
        if (states.at(current).compare("files")==0) {
            if (need_files) {
                clear();
                mvprintw(0,0,(char*)"Loading Files...");
                refresh();
                version=versions.at(selected+1);
                files=get_data(distro,distro+" getfiles "+version,"Select file:");
                bottom=files.size()-1;
                need_files=false;
                clear();
                refresh();
            }
            dialog(files,files.size(),distro);
        }        
        update_selection();
        refresh();
    }
    delwin(mainWindow);
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
    return 0;
}
