#include "window.h"

void update_selection(WINDOW* mainWindow, int* running, int* selection, int* update, int upperbound, int* selected) {
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
        case 65:
            // Down is up
            if (*selection > 0) (*selection)--;
            *update = 1;
            break;
        case 66:
            // Up is down
            if (*selection < upperbound) (*selection)++;
            *update = 1;
            break;
        case 10:
            *selected = 1;
            break;
    }
}

int cleanup(WINDOW* mainWindow) {
    int code = delwin(mainWindow);
    endwin(); // Exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
    return code;
}

void dialog(char** menu, int len, int height, int width, int selection) {
    int line = 1;
    attron(COLOR_PAIR(3));
    mvprintw(0, 0, "%s", menu[0]);
    attron(COLOR_PAIR(1));
    for (int i = 1; i < len; i++) {
        if (i - 1 == selection) {
            attron(COLOR_PAIR(2));
            mvprintw(line+1, 0, "%s", menu[i]);
            attron(COLOR_PAIR(1));
        }
        else mvprintw(line+1, 0, "%s", menu[i]);
        line++;
    }
    refresh();
}
