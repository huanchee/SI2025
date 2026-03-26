#ifndef STRINGEK_MOD
#define STRINGEK_MOD


#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


void mid_hor(WINDOW *win, int *x, int len);

void end_screen(WINDOW *win);

int choice_menu(WINDOW *parent, const char *options[], int opts, const char *title);

#endif