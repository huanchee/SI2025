#ifndef GAMEMOD_H
#define GAMEMOD_H


#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PROJ 3
#define MAX_INV 5
#define INV_SPEED 5*MAX_INV
#define MAX_M 4

typedef struct {
    char name[26];
    int points;
} playerOut;

typedef struct {
    char name[26];
    int invaderPlus;
    bool isMeteor;
} RunInfo;

typedef struct {
    int y, x;
    int active;
} Proj;

typedef struct {
    int y, x;
    /*
    ===  |y : x, y :  x+1, y :  x+2|
     U   |          x : y+1        |
    */
    int alive;
} Invader;

typedef struct {
    int y, x;
    int hp;
    int active;
} Meteor; 

bool collision(int i, int j, Proj pro[], Invader inv[]);

bool collision_meteor(int i, int j, Proj pro[], Meteor met[]);

bool free_space(int x_pos, Invader inv[], int invaderPlus);

bool free_space_meteor(int x_pos, Meteor inv[]);

void init_proj(Proj projectiles[]);

void init_inv(Invader invaders[], int invaderplus);

void init_meteors(Meteor meteors[], int go);

bool meteor_hit(int headx, int metx, int mety);

int step_meteor(WINDOW *win, Meteor meteors[], int * stpmeteor, int headx);

int step_invader(WINDOW *win, Invader invaders[], int * stpinvader, int invplus, int heady, int stp_meteor);

playerOut jatek(RunInfo fromPlayer);

#endif