#pragma once

#include <ncurses.h>
#include <string.h>

void update_selection(WINDOW* mainWindow, int* running, int* selection, int* update, int upperbound, int* selected);
int cleanup(WINDOW* window);
void dialog(char** menu, int len, int height, int width, int selection);
