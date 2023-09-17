#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <ncurses.h>
#include <iostream>
#include "network.h"
#include "window.h"

// Global Vars
bool running=true;
bool need_update=true;
int key; // Key id
int current=0; // Current menu
int current_selection=0; // Selected item
int bottom; // Bottom location (length of menu list)
std::vector<std::string> menu;
std::string current_url = "/pub/linux/kernel/";
std::string version;
std::string distro = "";

void update_selection(WINDOW * mainWindow) {
    key = getch();
    refresh();
    switch (key) {
        case 66:         
            if (current_selection<bottom-1) current_selection++;
            break;
        case 65:
            if (current_selection>0) current_selection--;
            break;
        case 10:
            if (current==2 && distro.compare("Linux Kernel")!=0) version=menu.at(current_selection+1);
            if (current < 3 && !(distro.compare("Linux Kernel")==0)) {
                current++;
                need_update=true;
            }
            if (distro.compare("Linux Kernel")==0) {
                if (menu.at(current_selection+2).compare("../")==0) {
                    while (true) {
                        current_url.pop_back();
                        if (current_url.back()=='/') break;
                    }
                }
                else current_url+=menu.at(current_selection+2);
                if (current_url.back() != '/') {
                    running=false;
                    cleanup(mainWindow);
                }
                else {
                    current++;
                    need_update=true;
                }
            }
            if (current==1 && distro.compare("")==0) distro=menu.at(current_selection+1); 
            if (current==3 && distro.compare("Linux Kernel")!=0) running=false;
            else if (distro.compare("Linux Kernel")!=0) current_selection=0;
            clear();
            break;
        case 113:
            running=false;
            cleanup(mainWindow);
            exit(0);
            break;
    }
}

void dialog(std::vector<std::string> menu,int length,int title_length,int height,int width) {
    int line=1;
    int column=0;
    attron(COLOR_PAIR(3));
    mvprintw(0,0,menu.at(0).c_str());
    if (title_length==2) mvprintw(1,0,("Index of "+menu.at(1)).c_str());
    for (int i=title_length; i<length; i++) {
        if (current_selection+title_length==i) {
            attron(COLOR_PAIR(1));
            for (int j=menu.at(0).size(); j<width; j++) mvprintw(0, j, (char*)" ");
            attron(COLOR_PAIR(2));
            mvprintw(0, width-menu.at(i).size()-1, menu.at(i).c_str());
        }
        else attron(COLOR_PAIR(1));
        if (line+title_length+1==height) {
            line=1;
            column+=19;
        }
        if (menu.at(i).size()+column < width) {
            if (menu.at(i).size() > 15) {
                std::string message = menu.at(i).substr(0,15)+="...";
                mvprintw(line+title_length,column,message.c_str());
            }
            else mvprintw(line+title_length,column,menu.at(i).c_str());
        }
        line++;
    }
    refresh();
}

int main() {
    std::vector<std::string> links;
    int height;
    int width;
    WINDOW * mainWindow;
    if ((mainWindow = initscr()) == NULL) {
        std::cout << "Failed to start ncurses" << std::endl;
        exit(1);
    } // start curses mode or exit on error
    getmaxyx(mainWindow, height, width);
    start_color();
    use_default_colors();
    init_pair(1,COLOR_WHITE,-1); // 1: No highlight
    init_pair(2,COLOR_BLACK,COLOR_WHITE); // 2: highlight
    init_pair(3,COLOR_RED,COLOR_CYAN); // 3: title highlight
    noecho();
    curs_set(0);
    clear();
    while (running) {
        // Main screen for everything
        if (current==0) {
            if (need_update) {
                menu=get_data("","menu","Select a Distro:", mainWindow);
                bottom=menu.size()-1;
                need_update=false;
            }
            dialog(menu,menu.size(),1,height,width); 
        }
        // Every distro besides Linux Kernel
        if (distro.compare("Linux Kernel")!=0) {            
            if (need_update) {
                clear();
                printw("Loading...");
                refresh();
                if (current==1) {
                    menu = get_data(distro, distro+" getvers","Select Versions:", mainWindow);
                }
                if (current==2) {
                    std::vector<std::string> filesWLinks = get_data(distro, distro+" getfiles "+version,"Select File:", mainWindow);
                    menu.clear();
                    for (int i=0; i<filesWLinks.size(); i++) {
                        if (i%2==1 || i==0) {
                            menu.push_back(filesWLinks.at(i));
                        }
                        else links.push_back(filesWLinks.at(i));
                    }
                }
                bottom=menu.size()-1;
                need_update = false;
                clear();
                refresh();
            }
            dialog(menu,menu.size(),1,height,width);
        }
        // For the linux kernel
        // Basically a file server browser
        if (distro.compare("Linux Kernel")==0) {
            if (need_update) {
                clear();
                printw("Loading...");
                menu=get_data(distro,distro+" "+current_url,current_url, mainWindow);
                bottom=menu.size()-2;
                need_update=false;
            }
            dialog(menu,menu.size(),2,height,width);
        }
        update_selection(mainWindow);
        refresh();
    }
    cleanup(mainWindow);
    if (distro.compare("Ubuntu")==0) download_file(links.at(current_selection), menu.at(current_selection+1));
    if (distro.compare("Linux Kernel")==0) download_file("https://mirrors.edge.kernel.org"+current_url, menu.at(current_selection+2));
    return 0;
}
