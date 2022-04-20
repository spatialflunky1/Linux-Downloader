#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <ncurses.h>
#include <iostream>

int key=0;
bool running=true;
bool need_update=true;
int current=0;
int current_selection=0;
int selected=0;

void update_selection() {
    key = getch();
    refresh();
    switch (key) {
        case 66:         
            current_selection++;
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

std::vector<std::string> get_versions(std::string title, std::string request) {
    std::vector<std::string> versions;
    std::string url = "72.231.177.233";
    int port = 80;
    httplib::Client cli(url,port);
    // Request ubuntu versions from the server
    auto res = cli.Post("/", request, "text/plain");
    // If data gets returned
    if (res) {
        std::string body_text = res->body;
        if (title.compare((std::string)"Ubuntu")==0) {
            versions.push_back((std::string)"Select Version:");
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
    std::vector<std::string> menu = {"Select a distro:",
                                      "Ubuntu",
                                      "Linux Kernel"}; // format: {title,item1,item2,etc}
    std::vector<std::string> states = {"distro","version","files"};
    int length1 = menu.size();
    std::vector<std::string> versions;
    int length2;
    std::string distro;
    initscr(); // start curses mode
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLACK); // 1: No highlight
    init_pair(2,COLOR_BLACK,COLOR_WHITE); // 2: highlight
    init_pair(3,COLOR_RED,COLOR_CYAN); // 3: title highlight
    noecho();
    curs_set(0);
    clear();
    while (running) {
        if (states.at(current).compare("distro")==0) {
            dialog(menu,length1,distro); 
        }
        if (states.at(current).compare("version")==0) {
            if (need_update) {
                clear();
                mvprintw(0,0,(char*)"Loading Versions...");
                refresh();
                distro=menu[selected+1];
                versions = get_versions(distro, distro+" getvers");
                length2 = versions.size();
                need_update = false;
                clear();
                refresh();
            }
            dialog(versions,length2,distro);
        }
        if (states.at(current).compare("files")==0) {
            if (menu[selected+1].compare((std::string)"Ubuntu") == 0) {         
            
            }
        }        
        update_selection();
        refresh();
    }
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}
