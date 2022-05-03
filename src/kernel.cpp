#include "httplib.h"
#include <ncurses.h>
#include <iostream>


// Global Vars
int key;
bool running=true;
bool needs_update=true;
int current=0;
int current_selection=0;
int selected;
int bottom;
std::string current_url = "/pub/";
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
            break;
        case 65:
            if (current_selection>0) current_selection--;
            key=0;
            break;
        case 10:
            current++;
            current_url+=menu.at(current_selection+2);
            current_selection=0;
            needs_update=true;
            clear();
            break;
        case 113:
            running=false;
            cleanup(mainWindow);
            exit(0);
            break;
    }
}

std::vector<std::string> get_directory(std::string directory) {
    std::vector<std::string> directories;
    httplib::Client cli(url,port);
    auto res = cli.Post("/", ("linux kernel "+directory).c_str(), "text/plain");
    if (res) {
        std::string body_text = res->body;
        directories.push_back("https://mirrors.edge.kernel.org/");
        directories.push_back(directory);
        std::stringstream s_stream(body_text);
        while(s_stream.good()) {
            std::string substr;
            getline(s_stream,substr,',');
            directories.push_back(substr);
        }
    }
    else {
        std::string message = (std::string)"Error: "+httplib::to_string(res.error())+(std::string)"\n";
        endwin();
        std::cout << message << std::endl;
        exit(0);
    }
    return directories;
}

void dialog(std::vector<std::string> menu, int length) {
    attron(COLOR_PAIR(3)); 
    mvprintw(0,0,menu.at(0).c_str());
    mvprintw(1,0,("Index of "+menu.at(1)).c_str());
    for (int i=2; i<length; i++) {
        if (current_selection+2==i) attron(COLOR_PAIR(2));
        else attron(COLOR_PAIR(1));
        mvprintw(i+2,0,menu.at(i).c_str());
    }
    refresh();
}

int main() {
    WINDOW * mainWindow;
    if ((mainWindow = initscr()) == NULL) {
        std::cout << "Failed to start ncurses" << std::endl;
        exit(1);
    } // start curses mode or exit on error
    start_color();
    use_default_colors();
    init_pair(1,COLOR_BLUE,COLOR_BLACK); // 1: No highlight
    init_pair(2,COLOR_CYAN,COLOR_MAGENTA); // 2: highlight
    init_pair(3,COLOR_BLACK,COLOR_WHITE); // 3: title highlight
    wbkgd(mainWindow, COLOR_PAIR(1));
    noecho();
    curs_set(0);
    clear();
    while (running) {
        if (needs_update) {
            menu = get_directory(current_url);
            bottom=menu.size()-2;
            needs_update=false;
        }
        dialog(menu,menu.size());
        update_selection(mainWindow);
    }
    cleanup(mainWindow);
    return 0;
}
