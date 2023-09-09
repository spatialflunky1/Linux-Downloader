#include "window.h"

void cleanup(WINDOW * mainWindow) {
    delwin(mainWindow);
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}
