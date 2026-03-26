#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include "leaderboard.h"
#include "debugmalloc.h"


void freelist(LeaderList  * eleje) {
    LeaderList * iter = eleje;
    while (iter != NULL) {
        LeaderList *next = iter->next; // kovetkezo elem
        free(iter);
        iter = next;
    }
}

bool vannagyobb(LeaderList * head, const char* name, int currentPoints) {
    LeaderList * temp = head;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0 && temp->points >= currentPoints) {
            return true;  // ez a nev mar szerepel nagyobb pontszammal
        }
        temp = temp->next;
    }
    return false;  // nem szerepel nagyobb pontszammal
}

LeaderList *loadLeadersFromFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fajl nyitas hiba");
        return NULL;
    }

    LeaderList *head = NULL;
    LeaderList *tail = NULL;

    char line[256];
    char tmpName[25];
    int tmpPoints;

    //soronkent olvas
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%24s %d", tmpName, &tmpPoints) != 2) {
            continue; // rossz minta kihagyasa
        }
        if (!vannagyobb(head, tmpName, tmpPoints)) {
            LeaderList *node = malloc(sizeof(LeaderList));
            if (!node) {
                perror("malloc hiba");
                fclose(fp);
                return head;
            }

            strncpy(node->name, tmpName, sizeof(node->name) - 1);
            node->name[sizeof(node->name) - 1] = '\0';
            node->points = tmpPoints;
            node->next = NULL;

            if (head == NULL) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
        }
    }
    fclose(fp);
    return head;
}

LeaderList * lista_vegere(LeaderList * eleje, char *nev, int pontjai) {
    if (!vannagyobb(eleje, nev, pontjai)) {
        
        LeaderList *uj = malloc(sizeof(LeaderList));

        if (eleje == NULL) {
            strncpy(uj->name, nev, sizeof(uj->name) - 1);
            uj->name[sizeof(uj->name) - 1] = '\0';
            uj->points = pontjai;
            uj->next = NULL;
            eleje = uj;

        } else {
            LeaderList * mozgo = eleje;
            while (mozgo->next != NULL) {
                mozgo = mozgo->next;
            }
            strncpy(uj->name, nev, sizeof(uj->name) - 1);
            uj->name[sizeof(uj->name) - 1] = '\0';
            uj->points = pontjai;
            uj->next = NULL;
            mozgo->next = uj;
        }

    }
    return eleje;
}

int lenlista(LeaderList * eleje) {
    LeaderList *mozgo = eleje;
    int cnt = 0;
    while (mozgo != NULL) {
        cnt ++;
        mozgo = mozgo->next;
    }
    return cnt;
}


int digit(int n) {
    int count = 0;

    if (n == 0) return 1;       

    while (n != 0) {
        n /= 10;               
        count++;               
    }
    return count;
}

void sortLeaders(LeaderList *eleje) {
    // csokkeno sorrendbe allitja a jatekos rekordokat
    if (eleje == NULL) return;

    int csere;
    LeaderList *ptr;
    LeaderList *utolsoSorted = NULL;

    do {
        csere = 0;
        ptr = eleje;

        while (ptr->next != utolsoSorted) {
            if (ptr->points < ptr->next->points) {
                // pontok kicserelese
                int tmpPoints = ptr->points;
                ptr->points = ptr->next->points;
                ptr->next->points = tmpPoints;

                // nevek kicserelese
                char tmpName[25];
                strncpy(tmpName, ptr->name, sizeof(tmpName));
                strncpy(ptr->name, ptr->next->name, sizeof(ptr->name));
                strncpy(ptr->next->name, tmpName, sizeof(ptr->next->name));

                csere = 1;
            }
            ptr = ptr->next;
        }
        utolsoSorted = ptr;
    } while (csere);
}

void printleaderboard(LeaderList *eleje) {
    int x, y;
    getmaxyx(stdscr, y, x);
    int w = 51;
    int h = 40;
    
    WINDOW *wl = newwin(h, w, (y-h)/2, (x-w)/2);
    box(wl, 0, 0);
    int mid_correct_y = (h-lenlista(eleje))/2;
    int linect = 0;
    for (LeaderList *p = eleje; p != NULL && linect < 20; p = p->next) {
        int templ_name = strlen(p->name) + 3;
        int templ_points = digit(p->points);
        
        mvwprintw(wl, mid_correct_y + linect, (w-templ_name-templ_points)/2, "%d. %s: %d", linect+1, p->name, p->points);
        linect ++;
    }
    wrefresh(wl);
    wgetch(wl);
    delwin(wl);  
}

void linkedlistToFile(LeaderList *eleje, char *filename){
    FILE *output = fopen(filename, "w");
    if (output == NULL) perror("output txt hiba nyitasnal");

    LeaderList *temp = eleje;
    // csak 20x fut le mert annal tobb nevet nem akarunk kiirni a fajlba
    for (int i = 0; i < 20 && temp != NULL; i++) {
        fprintf(output, "%s %d\n", temp->name, temp->points);
        temp = temp->next;
    }
    fclose(output);
}