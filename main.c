#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "game_mod.h"
#include "leaderboard.h"
#include "stringek_mod.h"
#include "debugmalloc.h"



void preGameMenu(WINDOW *win, RunInfo *fromPlayer){
    int H, W;
    getmaxyx(win, H, W);

    
    int ibox_w = (W > 34 ? 30 : (W > 8 ? W - 4 : W));
    int ibox_h = 3;
    int ibox_y = H / 2 - 2;
    if (ibox_y < 1) ibox_y = 1; // keep space for the label
    int ibox_x = (W - ibox_w) / 2;

    // Label es input box (derived window relative to 'win')
    box(win, 0, 0);
    mvwprintw(win, ibox_y - 1, ibox_x, "NAME:");
    wrefresh(win);

    WINDOW *ibox = derwin(win, ibox_h, ibox_w, ibox_y, ibox_x);
    box(ibox, 0, 0);
    keypad(ibox, TRUE);
    wmove(ibox, 1, 1);
    wrefresh(ibox);

    // text input (max 25 chars)
    echo();
    curs_set(1);
    wgetnstr(ibox, fromPlayer->name, 25);
    noecho();
    curs_set(0);

    delwin(ibox);
    wclear(win);

    // LEVEL menu ('win' ablak leszarmazottja, benne relativ)
    const char *options[] = {"BASIC", "HARD", "MADNESS"};

    int choice = choice_menu(win, options, 3, "LEVEL");
    // selection
    if (choice == 1) {          // BASIC
        fromPlayer->invaderPlus = 0;
        fromPlayer->isMeteor = false;
    } else if (choice == 2) {   // HARD
        fromPlayer->invaderPlus = 0;
        fromPlayer->isMeteor = true;
    } else {                    // MADNESS
        fromPlayer->invaderPlus = 2;
        fromPlayer->isMeteor = true;
    }

    // clear
    werase(win); wrefresh(win);
}

int menu() { //gives back the choice
    box(stdscr, 0, 0);
    const char *menu[] = {"Jatek", "Leaderboard", "Kilepes"};
    const char neve[] = "Menu";
    return choice_menu(stdscr, menu, 3, neve);
}

int main() {
    //set basics
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    //print_title(stdscr, 4);
    //Load Leaderboard
    LeaderList *eleje = loadLeadersFromFile("data.txt");
    //Main menu
    RunInfo fromPlayer;
    playerOut pInfo;
    while (1) {
        clear();
        box(stdscr, 0, 0);
        refresh();
        int choice = menu();
        switch (choice) {
            case 1: 
                // jatek elotti infok begyujtese
                preGameMenu(stdscr, &fromPlayer);
                // jatek ami visszaadja a pontokat es a jatekos nevet
                pInfo = jatek(fromPlayer);
                char name[26];
                int point = pInfo.points;

                strncpy(name, pInfo.name, sizeof(pInfo.name)-1);
                name[sizeof(pInfo.name)-1] = '\0';
                // jatekos adatainak hozzaadasa a listahoz
                eleje = lista_vegere(eleje, name, point);
                end_screen(stdscr);

                break;
            case 2:
                sortLeaders(eleje);
                printleaderboard(eleje);
                break;
            case 3:
                // kilepes elott a data.txt modositasa
                linkedlistToFile(eleje, "data.txt");
                endwin();
                freelist(eleje);
                exit(0); 
                break;
            default: 
                continue; 
                break;
        }
    }
    
    freelist(eleje);
    endwin();
    return 0;
}
