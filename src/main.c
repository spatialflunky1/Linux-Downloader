#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "network.h"

#define FIRSTMENU_LEN 5

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
    char *firstMenu[5];
    firstMenu[0] = "Select a Distribution:";
    firstMenu[1] = "Arch";
    firstMenu[2] = "Gentoo";
    firstMenu[3] = "Ubuntu";
    firstMenu[4] = "Linux Kernel";

    WINDOW* mainWindow;
    int width;
    int height;
    // 0: not running, 1: running
    int running = 1;
    // Selected menu item 
    int selection = 0;
    // Indicates enter key pressed
    int selected = 0;
    // Current menu
    // 0: Select distro
    // 1: Arch (ex. x86, NOT the distro)
    // 2: Version
    // 3: Files
    int menu_num = 0;
    // Selected distro
    // 0: None
    // 1: Arch
    // 2: Gentoo
    // 3: Ubuntu
    // 4: Linux Kernel
    int distro = 0;
    // 1: Update screen on next loop
    int update = 1;

    // General variables to be set
    int title_len = 1;
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
        // Select a Distro:
        if (update) {
            // Menu/title len is constant so plugged in so as not to create an unnessesary
            dialog(firstMenu, 5, height, width, selection);
            update = 0;
        }
        if (selected) {
            selected = 0;
            distro = selection + 1;
            selection = 0;
            if (distro == 1) menu_num = 3;
            else menu_num = 1;
            break;
        }
        update_selection(mainWindow, &running, &selection, &update, FIRSTMENU_LEN - title_len - 1, &selected);
        refresh(); // Refresh curses window
    }
    if (menu_num == 1) {
        // Get architecture
        // printf("%s\n", menu[distro + title_len - 1]);
    }

    else if (menu_num == 2) {
        // Get version
    
    } 

    else if (menu_num == 3) {
        // Select file
        char** files = NULL;
        int files_len = 0;
        get_files(distro, &files, &files_len);
        
        /*
        while (running) {
            if (update) {
                
            }
            //update_selection();
            refresh();
        }
        */

        for (int i = 0; i < files_len; i++) {
            free(files[i]);
        }
        free(files);
    }
    cleanup(mainWindow);

    return 0;
}
