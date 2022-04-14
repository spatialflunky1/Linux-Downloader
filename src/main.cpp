#include <ncurses.h>
#include <iostream>

int key=0;
bool running=true;
int current=0;
int current_selection=0;

void update_selection() {
    key = getch();
    refresh();
    switch (key) {
        case 66:         
            current_selection++;
            key=0;
            break;
        case 65:
            if (current_selection>0) current_selection--;
            key=0;
            break;
        case 10:
            current++;
            clear();
            break;
        case 113:
            running=false;
            break;
    }
}

void dialog(char **menu, int length) {
    attron(COLOR_PAIR(3));
    mvprintw(0,0,menu[0]);
    for (int i=1; i<length; i++) {
        if (current_selection+1==i) attron(COLOR_PAIR(2));
        else attron(COLOR_PAIR(1));
        mvprintw(i+1,0,menu[i]);
    }
    refresh();
}

int main() {
    char *menu1[] = {(char*)"Select a distro:",
                     (char*)"Ubuntu",
                     (char*)"Gentoo",
                     (char*)"Linux Kernel"}; // format: {title,item1,item2,etc}
    int length1 = sizeof(menu1)/sizeof(menu1[0]);
    char *menu2[] = {(char*)"Select Version:\n"};
    int length2 = sizeof(menu2)/sizeof(menu2[0]);

    initscr(); // start curses mode
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLACK); // 1: No highlight
    init_pair(2,COLOR_BLACK,COLOR_WHITE); // 2: highlight
    init_pair(3,COLOR_RED,COLOR_CYAN); // 3: title highlight
    noecho();
    curs_set(0);
    clear();
    while (running) {
        switch (current) {
            case 0:
                dialog(menu1,length1); 
                break;
            case 1:
                dialog(menu2,length2);
                break;
        }        
        update_selection();
        refresh();
    }
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}
