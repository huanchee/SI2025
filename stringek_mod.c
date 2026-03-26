#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "stringek_mod.h"
#include "debugmalloc.h"

void mid_hor(WINDOW *win, int *x, int len) {
    //visszaadja a relativ kozep x-et
    *x = (getmaxx(win) - len) / 2;
}


void end_screen(WINDOW *win){
    // losing screen
    wclear(win);
    int posxx = 0;
    mid_hor(win, &posxx, 34);
    mvwprintw(win, 10, posxx, "Game ended - press ENTER to continue");     
    box(win, 0, 0);
    wrefresh(win);
    int act = 1;
    char chh; 
    while(act) {
        chh = wgetch(win);
        if (chh == '\n') {
            break;
        } else {
            continue;
        }
    }
}

int choice_menu(WINDOW *parent, const char *options[], int opts, const char *title) {
    // makes a subwindow to a given one, relative sizing, given opt, opt len and title
    int H, W;
    getmaxyx(parent, H, W);
    int m_w = (W > 26 ? 22 : (W > 8 ? W - 4 : W));
    int m_h = opts + 4;
    int m_y = (H - m_h) / 2; if (m_y < 0) m_y = 0;
    int m_x = (W - m_w) / 2;

    WINDOW *mwin = derwin(parent, m_h, m_w, m_y, m_x);
    box(mwin, 0, 0);
    keypad(mwin, TRUE);
    mvwprintw(mwin, 1, (m_w - (int)strlen(title)) / 2, "%s", title);

    int choice = 0;
    int ch;
    while (1) {
        for (int i = 0; i < opts; ++i) {
            if (i == choice) wattron(mwin, A_REVERSE);
            mvwprintw(mwin, i + 2, 2, "%s", options[i]);
            mvwprintw(stdscr, 3, 3, "%d", choice);
            if (i == choice) wattroff(mwin, A_REVERSE);
            box(stdscr, 0, 0);
        }
        wrefresh(mwin);

        ch = wgetch(mwin);
        if (ch == KEY_UP) {
            if (choice > 0) choice--;
        } else if (ch == KEY_DOWN) {
            if (choice < opts - 1) choice++;
        } else if (ch == '\n') {
            break;
        }
    }


    // clear
    werase(mwin); wrefresh(mwin); delwin(mwin);
    werase(parent); wrefresh(parent);
    return choice+1; // 0, ha valami hiba, 1-2-3...-opt
}
