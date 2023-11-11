#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "window.h"
#include "network.h"

#define FIRSTMENU_LEN 5

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
    // Data wise any reference to this variable should be +1
    int selection = 0;
    // Indicates enter key pressed
    int selected = 0;
    // Current menu
    // 0: Select distro
    // 1: Arch (ex. x86, NOT the distro)
    // 2: Version
    // 3: Files
    // 4: Filetype (for linux kernel to minimize menu selections)
    int menu_num = 0;
    // Selected distro
    // 0: None
    // 1: Arch
    // 2: Gentoo
    // 3: Ubuntu
    // 4: Linux Kernel
    int distro = 0;
    // The URL string is built piecewise
    char* URL = NULL;
    int URL_len = 0;
    // 1: Update screen on next loop
    int update = 1;
    // Window return code
    int code = 1;
    // Files list is global in order to be across multiple menus
    char** files = NULL;
    int files_len = 0;
    // Files without the filetypes
    char** ftypes = NULL;
    int ftypes_len = 0;
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
            // firstMenu is constant so the length is plugged in as not to create an unnessesary variable
            dialog(firstMenu, 5, height, width, selection);
            update = 0;
        }
        if (selected) {
            clear();
            selected = 0;
            update = 1;
            distro = selection + 1;
            selection = 0;
            switch (distro) {
                case 1: {
                    append_string_string(ARCH_URL, &URL, &URL_len);
                    menu_num = 2;
                    break;
                }
                case 2: {
                    append_string_string(GENTOO_URL, &URL, &URL_len);
                    menu_num = 1;
                    break;
                }
                case 3: {
                    append_string_string(UBUNTU_URL, &URL, &URL_len);
                    menu_num = 2; 
                    break;
                }
                case 4: {
                    append_string_string(LINUX_URL, &URL, &URL_len);
                    menu_num = 2;
                    break;
                }
            }
            break;
        }
        update_selection(mainWindow, &running, &selection, &update, FIRSTMENU_LEN - 2, &selected);
        refresh();
    }

    if (menu_num == 1) {
        // Get architecture
        char** archs = NULL;
        int archs_len = 0;
        append_string_array("Select Architecture: ", &archs, &archs_len);
        get_directories(URL, &archs, &archs_len, 0);
        while (running) {
            if (update) {
                dialog(archs, archs_len, height, width, selection);
                update = 0;
            }
            if (selected) {
                clear();
                selected = 0;
                update = 1;
                append_string_string(archs[selection + 1], &URL, &URL_len);
                append_string_string("/", &URL, &URL_len); // Appending a single character here breaks the string so it needs to append as a string instead, no idea why
                if (distro == 2) {
                    append_string_string("autobuilds/", &URL, &URL_len);
                }
                selection = 0;
                menu_num++;
                break;
                
            }
            update_selection(mainWindow, &running, &selection, &update, archs_len - 2, &selected);
            refresh();
        }
        // First element is a text segment string, cannot free it
        if (archs != NULL) {
            for (int i = 1; i < archs_len; i++) {
                free(archs[i]);
            }
            free(archs);
        }
    }

    if (menu_num == 2) {
        // Get version
        char** versions = NULL;
        int vers_len = 0;
        append_string_array("Select Version: ", &versions, &vers_len);
        get_directories(URL, &versions, &vers_len, (distro==2||distro==3) ? 1 : distro==4 ? 2 : 0);
        while (running) {
            if (update) {
                dialog(versions, vers_len, height, width, selection);
                update = 0;
            }
            if (selected) {
                clear();
                selected = 0;
                update = 1;
                append_string_string(versions[selection + 1], &URL, &URL_len);
                append_string_string("/", &URL, &URL_len);
                selection = 0;
                menu_num++;
                break;
            }
            update_selection(mainWindow, &running, &selection, &update, vers_len - 2, &selected);
            refresh();
        }
        if (versions != NULL) {
            for (int i = 1; i < vers_len; i++) {
                free(versions[i]);
            }
            free(versions);
        }
    } 

    if (menu_num == 3) {
        // Select file
        append_string_array("Select File:", &ftypes, &ftypes_len);
        get_files(URL, &files, &files_len, distro, &ftypes, &ftypes_len); 
        while (running) {
            if (update) {
                dialog(ftypes, ftypes_len, height, width, selection);
                update = 0;
            }
            if (selected) {
                if (distro != 4) {
                    code = cleanup(mainWindow);
                    download_file(distro, URL, files[selection + 1]);
                }
                else {
                    clear();
                    menu_num++;
                    update = 1;
                    selected = 0;
                }
                break;
            }
            update_selection(mainWindow, &running, &selection, &update, ftypes_len - 2, &selected);
            refresh();
        }
    }

    if (menu_num == 4) {
        // Select filetype
        char** filetypes = NULL;
        int filetypes_len = 0;
        // Need to use a different selection variable to save selected file
        int filetype_selection = 0;

        append_string_array("Select Filetype:", &filetypes, &filetypes_len);
        for (int i = 0; i < files_len; i++) {
            if (sub_compare(ftypes[selection + 1], files[i]) == 1) {
                if (isdigit(ftypes[selection+1][strlen(ftypes[selection+1]) - 1])) {
                    if (files[i][strlen(ftypes[selection+1]) + 1] != 'p') {
                        append_string_array(files[i], &filetypes, &filetypes_len); 
                    }
                }
                else {
                    append_string_array(files[i], &filetypes, &filetypes_len);
                }
            }
        }
        while (running) {
            if (update) {
                dialog(filetypes, filetypes_len, height, width, filetype_selection);
                update = 0;
            }
            if (selected) {
                code = cleanup(mainWindow);
                download_file(distro, URL, filetypes[selection + 1]);
                break;
            }
            update_selection(mainWindow, &running, &filetype_selection, &update, filetypes_len - 2, &selected);
            refresh();
        }

        if (files != NULL) {
            for (int i = 0; i < files_len; i++) {
                free(files[i]);
            }
            free(files);
        }
        if (ftypes != NULL) {
            for (int i = 1; i < ftypes_len; i++) {
                free(ftypes[i]);
            }
            free(ftypes);
        }
    }
    if (code != 0) {
        cleanup(mainWindow);
    }
    if (URL != NULL) {
        free(URL);
    }

    return 0;
}
