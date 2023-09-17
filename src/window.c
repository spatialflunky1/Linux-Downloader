#include "window.h"

void update_selection(WINDOW* mainWindow, int* running, int* selection, int* update, int upperbound) {
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
    }
}

void cleanup(WINDOW* mainWindow) {
    delwin(mainWindow);
    endwin(); // Exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}

void dialog(char** menu, int len, int title_len, int height, int width, int selection) {
    int line = 1;
    attron(COLOR_PAIR(3));
    mvprintw(0, 0, "%s", menu[0]);
    attron(COLOR_PAIR(1));
    for (int i = title_len; i < len; i++) {
        if (i - title_len == selection) {
            attron(COLOR_PAIR(2));
            mvprintw(line+title_len, 0, "%s", menu[i]);
            attron(COLOR_PAIR(1));
        }
        else mvprintw(line+title_len, 0, "%s", menu[i]);
        line++;
    }
    refresh();
}