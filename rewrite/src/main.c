#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "window.h"

void menu_append(char* menuItem, char*** menu, int* len) {
    char** newMenu = malloc((*len) + 1);
    if (newMenu == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    if (*menu != NULL) {
        for (int i = 0; i < (*len); i++) {
            newMenu[i] = malloc((strlen((*menu)[i]) + 1) * sizeof(char));
            strcpy(newMenu[i], (*menu)[i]);
        }
        free(*menu);
    }
    newMenu[*len] = malloc((strlen(menuItem) + 1) * sizeof(char)); 
    strcpy(newMenu[*len], menuItem); 
    (*len)++;
    *menu = newMenu;
}

int main(void) {
    // Create the first menu
    char** firstMenu = NULL;
    int firstMenu_len = 0;
    menu_append("Select a Distro:", &firstMenu, & firstMenu_len);
    menu_append("Arch", &firstMenu, & firstMenu_len);
    menu_append("Gentoo", &firstMenu, & firstMenu_len);
    menu_append("Ubuntu", &firstMenu, & firstMenu_len);
    menu_append("Linux Kernel", &firstMenu, & firstMenu_len);

    WINDOW* mainWindow;
    int width;
    int height;
    // 0: not running, 1: running
    int running = 1;
    // Selected menu item 
    int selection = 0;
    // Current menu
    // 0: Select distro
    int menu = 0;
    // Selected distro
    // 0: None
    // 1: Arch
    // 2: Gentoo
    // 3: Ubuntu
    // 4: Linux Kernel
    int distro = 0;
    if ((mainWindow = initscr()) == NULL) {
        fprintf(stderr, "Failed to start ncurses\n");
        return 1;
    } // start curses mode or exit on error
    getmaxyx(mainWindow, height, width);

    // Initialize colors
    start_color();
    use_default_colors();
    init_pair(1,COLOR_WHITE,-1); // 1: No highlight
    init_pair(2,COLOR_BLACK,COLOR_WHITE); // 2: highlight
    init_pair(3,COLOR_RED,COLOR_CYAN); // 3: title highlight

    noecho(); // Characters typed do not appear
    curs_set(0);
    clear();
    // Main loop
    while (running) {
        if (menu == 0) {
            // Select a Distro:
            dialog(firstMenu, firstMenu_len, 16, height, width);
        }
        update_selection(mainWindow, &running);
        refresh(); // Refresh curses window
    }
    cleanup(mainWindow);

    return 0;
}
