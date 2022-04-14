#include <ncurses.h>
#include <iostream>

int main(void) {
    int key=0;
    initscr();
    while (key != 113) {
        key = getch();
        clear();
        printw(std::to_string(key).c_str());
        refresh();
    }
    endwin();
}
