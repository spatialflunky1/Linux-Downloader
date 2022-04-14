#include <ncurses.h>

int key=0;
bool running=true;
int current=0;
int current_selection=0;

void update_selection() {
    key = getch();
    refresh();
    switch (key) {
        case 66:         
            current_selection--;
            key=0;
            break;
        case 65:
            current_selection++;
            key=0;
            break;
        case 113:
            running=false;
            break;
    }
}

void dialog() {
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(0,0,"Testing no highlight");
    
    attron(COLOR_PAIR(2));
    mvprintw(1,0,"Testing highlight");
    refresh();
}

int main() {
    initscr(); // start curses mode
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    noecho();
    while (running) {
        dialog();
        update_selection();
        refresh();
    }
    endwin(); // exit curses mode
    printf("\x1b[2J"); // clear screen
    printf("\x1b[d"); // return to home position
}
