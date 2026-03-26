#ifndef LEADERBOARD_H
#define LEADERBOARD_H


#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>

typedef struct LeaderList{
    char name[26];
    int points;
    struct LeaderList *next;
} LeaderList;

void freelist(LeaderList  * eleje);

bool vannagyobb(LeaderList * head, const char* name, int currentPoints);

LeaderList *loadLeadersFromFile(const char *filename);

LeaderList * lista_vegere(LeaderList * eleje, char *nev, int pontjai);

int lenlista(LeaderList * eleje);


int digit(int n);

void sortLeaders(LeaderList *eleje);

void printleaderboard(LeaderList *eleje);

void linkedlistToFile(LeaderList *eleje, char *filename);

#endif