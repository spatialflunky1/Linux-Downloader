#include <stdio.h>
#include <stdlib.h>
#include "window.h"

int main(void) {
    WINDOW* mainWindow;
    int width;
    int height;
    // 0: not running, 1: running
    int running = 1;
    // 
    int selection = 0;
    if ((mainWindow = initscr()) == NULL) {
        fprintf(stderr, "Failed to start ncurses\n");
        return 1;
    } // start curses mode or exit on error
    getmaxyx(mainWindow, height, width);
    noecho(); // Characters typed do not appear
    curs_set(0);
    clear();
    // Main loop
    while (running) {
        
        refresh(); // Refresh curses window
    }
    cleanup(mainWindow);

    return 0;
}
