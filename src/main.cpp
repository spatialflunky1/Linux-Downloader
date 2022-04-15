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
    if (title.compare((std::string)"Ubuntu") == 0) {
        printw((char*)"\n");
        for (int i=1; i<length-1; i+=2) {
            if (current_selection==(i-1)/2) attron(COLOR_PAIR(2));
            else attron(COLOR_PAIR(1));
            printw(((std::string)"\n"+menu[i]+(std::string)" ").c_str());
            printw(menu[i+1].c_str());
        }
    }
    else {
        for (int i=1; i<length; i++) {
            if (current_selection+1==i) attron(COLOR_PAIR(2));
            else attron(COLOR_PAIR(1));
            mvprintw(i+1,0,menu[i].c_str());
        }
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
                versions.push_back((std::string)entry["fullseriesname"]);
                versions.push_back((std::string)entry["version"]);
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
    std::vector<std::string> versions; // ubuntu format: {name,number,name,number,etc}
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
        switch (current) {
            case 0:
                dialog(menu,length1,distro); 
                break;
            case 1:
                if (need_update) {
                    clear();
                    mvprintw(0,0,(char*)"Loading Versions...");
                    refresh();
                    distro=menu[selected+1];
                    versions = get_versions(distro);
                    length2 = versions.size();
                    need_update = false;
                    clear();
                    refresh();
                }
                dialog(versions,length2,distro);
                break;
            case 2:
                if (menu[selected+1].compare((std::string)"Ubuntu") == 0) {
                    for (int i=1; i<versions.size(); i++) {
                        mvprintw(i-1,0,versions[i].c_str());
                    }
                }
                break;
        }        
        update_selection();
        refresh();
    }
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}
