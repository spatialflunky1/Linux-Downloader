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
int selected_distro=0;

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
            if (current < 1) current++;
            selected_distro=current_selection;
            current_selection=0;
            clear();
            break;
        case 113:
            running=false;
            break;
    }
}

void dialog(std::vector<std::string> menu, int length) {
    attron(COLOR_PAIR(3));
    mvprintw(0,0,menu[0].c_str());
    for (int i=1; i<length; i++) {
        if (current_selection+1==i) attron(COLOR_PAIR(2));
        else attron(COLOR_PAIR(1));
        mvprintw(i+1,0,menu[i].c_str());
    }
    refresh();
}

std::vector<std::string> get_versions(std::string title) {
    std::string url;
    std::string subdomain;
    if (title.compare((std::string)"Ubuntu")==0) {
        url = "https://api.launchpad.net";
        subdomain = "/devel/ubuntu/series";
    }
    httplib::Client cli(url);
    auto res = cli.Get(subdomain.c_str());
    std::vector<std::string> versions;
    if (res) {
        std::string body_text = res->body;
        nlohmann::json body = nlohmann::json::parse(body_text);       
        if (title.compare((std::string)"Ubuntu")==0) {
            versions.push_back((std::string)"Select Version:");
            for (int i=0; i<(int)body["total_size"]; i++) {        
                nlohmann::json entry = body["entries"][i];
                std::string version = (std::string)entry["fullseriesname"]+(std::string)" "+(std::string)entry["version"]+(std::string)"\n";  
                versions.push_back(version);
            }
        }
    }
    else {
        std::string message = (std::string)"Error: "+httplib::to_string(res.error())+(std::string)"\n";
        mvprintw(0,0,message.c_str());
    }
    return versions;
}

int main() {
    std::vector<std::string> menu = {"Select a distro:",
                                      "Ubuntu",
                                      "Gentoo",
                                      "Linux Kernel"}; // format: {title,item1,item2,etc}
    int length1 = menu.size();
    std::vector<std::string> versions;
    int length2;
    initscr(); // start curses mode
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLACK); // 1: No highlight
    init_pair(2,COLOR_BLACK,COLOR_WHITE); // 2: highlight
    init_pair(3,COLOR_RED,COLOR_CYAN); // 3: title highlight
    noecho();
    curs_set(0);
    clear();
    while (running) {
        switch (current) {
            case 0:
                dialog(menu,length1); 
                break;
            case 1:
                if (need_update) {
                    clear();
                    mvprintw(0,0,(char*)"Loading Versions...");
                    refresh();
                    versions = get_versions(menu[selected_distro+1]);
                    length2 = versions.size();
                    need_update = false;
                    clear();
                }
                dialog(versions,length2);
                break;
        }        
        update_selection();
        refresh();
    }
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}
