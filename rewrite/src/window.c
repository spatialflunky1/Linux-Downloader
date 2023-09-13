#include "window.h"

void update_selection(WINDOW* mainWindow, int* running) {
    // KEYS:
    // 113: Q
    // 66:  down arrow
    // 65:  up arrow
    // 10:  enter
    int key = getch();
    refresh();
    switch (key) {
        case 113:
            *running = 0;
            break;
    }
}

void cleanup(WINDOW* mainWindow) {
    delwin(mainWindow);
    endwin(); // Exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}

void dialog(char** menu, int len, int title_len, int height, int width) {

}
