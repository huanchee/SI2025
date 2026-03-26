#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "game_mod.h"
#include "debugmalloc.h"

bool collision(int i, int j, Proj pro[], Invader inv[]) {
    // projectile - invader collision
    if ((pro[i].x == inv[j].x+1 && pro[i].y == inv[j].y+1) || (pro[i].x == inv[j].x && pro[i].y == inv[j].y) || (pro[i].x == inv[j].x+2 && pro[i].y == inv[j].y)) {
        return true;
    }
    return false;
}

bool collision_meteor(int i, int j, Proj pro[], Meteor met[]) {
    // projectile - meteor collision
    if (j < MAX_M) {
        if ((pro[i].x == met[j].x || pro[i].x == met[j].x+1) && (pro[i].y == met[j].y+1 || pro[i].y == met[j].y)) {
            // printf("Meteor collision at: %d, %d || ", met[j].x, met[j].y);
            return true;
        }
    }
    return false;
}

bool free_space(int x_pos, Invader inv[], int invaderPlus) {
    // megnezi van-e eleg hely egy invader lerakasara
    for (int i = 0; i < MAX_INV+invaderPlus; i ++) {
        if ((inv[i].x == x_pos) || (inv[i].x == x_pos-1) || (inv[i].x == x_pos+1) || (inv[i].x == x_pos-2) || (inv[i].x == x_pos+2)) {
            return false;
        }
    }
    return true;
}

bool free_space_meteor(int x_pos, Meteor inv[]) {
    // megnezi van-e eleg hely egy invader lerakasara
    for (int i = 0; i < MAX_M; i ++) {
        if ((inv[i].x == x_pos) || (inv[i].x == x_pos-1) || (inv[i].x == x_pos+1) || (inv[i].x == x_pos-2) || (inv[i].x == x_pos+2)) {
            return false;
        }
    }
    return true;
}

void init_proj(Proj projectiles[]) {
    for (int i = 0; i < MAX_PROJ; i ++) {
        projectiles[i].active = 0;

    }
}

void init_inv(Invader invaders[], int invaderplus) {
    for (int i = 0; i < MAX_INV + invaderplus; i ++) {
        invaders[i].alive = 0;
    }
}

void init_meteors(Meteor meteors[], int go) {
    if (go == 0) {
        for (int i = 0; i < MAX_M; i ++) {
            meteors[i].active = 0;
            meteors[i].hp = 2;
        }
    } else {
        for (int i = 0; i < 3; i ++) {
            meteors[i].active = -1;
        }
    }
}



bool meteor_hit(int headx, int metx, int mety) {
    // checks if meteor collides with the player

    if (mety == 37) {
        if (metx == headx || metx == headx+1 || metx == headx-1 || metx+1 == headx || metx+1 == headx+1 || metx+1 == headx-1) {
            
            return true;
        } else {
            return false;
        }
    } else if (mety == 36) {
        if (metx == headx || metx+1 == headx) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

int step_meteor(WINDOW *win, Meteor meteors[], int * stpmeteor, int headx) {
    for (int i = 0; i < MAX_M; i ++) {
        if (meteors[i].active) {
            mvwprintw(win, meteors[i].y, meteors[i].x, "00");
            mvwprintw(win, meteors[i].y + 1, meteors[i].x, "00");
            // stp-balansz
            srand(time(NULL));
            if (*stpmeteor > INV_SPEED/2) {
                meteors[i].y = meteors[i].y + 1;
                *stpmeteor = 0;
            }
            ++ *stpmeteor;
            if (meteors[i].y > 37) {
                meteors[i].active = 0;
            }
            if (meteor_hit(headx, meteors[i].x, meteors[i].y) == true) {
                return 0;
            }
            
        }
    }
    return 1;
}

int step_invader(WINDOW *win, Invader invaders[], int * stpinvader, int invplus, int heady, int stp_meteor){
    // 0 ha veszit, 1 ha futhat tovabb a program
    for (int i = 0; i < MAX_INV+invplus; i ++) {
            if (invaders[i].alive) {
                mvwprintw(win, invaders[i].y, invaders[i].x, "===");
                mvwprintw(win, invaders[i].y + 1, invaders[i].x, " U");
                // fontos pont az invader - usleep balansz miatt
                if (*stpinvader > INV_SPEED) {
                    invaders[i].y = invaders[i].y + 1;
                    *stpinvader = 0;
                }
                ++ *stpinvader;
                if (invaders[i].y > heady-2) {

                    return 0;
                }
            }
        }
        return 1;
}

playerOut jatek(RunInfo fromPlayer) {
    box(stdscr, 0, 0);

    //SCREEN
    
    int h = 40, w = 51;

    int heady = 37;
    int headx = 25;

    WINDOW *win = newwin(h, w, (LINES-h)/2, (COLS-w)/2);
    WINDOW *score = newwin(h/5, w/5, (LINES-h)/2, (COLS-w)/2-(w/5));

    int points = 0;

    keypad(win, true);
    noecho();
    curs_set(0);
    cbreak();
    nodelay(score, true);
    nodelay(win, true);
    // SCREEN

    

    //INIT PROJECTILES
    Proj projectiles[MAX_PROJ];
    init_proj(projectiles);
    
    // INIT INVADERS
    int stp_invader = 0;
    Invader invaders[MAX_INV+fromPlayer.invaderPlus];
    init_inv(invaders, fromPlayer.invaderPlus);

    // INIT METEORS IF NEEDED {0 needed, -1 needed}
    int stp_meteor = 0;
    Meteor meteors[MAX_M];
    if (fromPlayer.isMeteor) {
        init_meteors(meteors, 0);
    } else {
        init_meteors(meteors, -1);
    }

    // start main game seqeunce
    
    int run = 1;
    int run2 = 1;
    while (run == 1 && run2 == 1) {
        werase(win);
        box(score, 0, 0);
        mvwprintw(win, heady, headx, "A");
        mvwprintw(win, heady+1, headx-1, "MMM");

        // small placement checker
        // mvwprintw(stdscr, 2,2, "x%d y%d", headx, heady);

        // SET invaders
        for (int i = 0; i < MAX_INV+fromPlayer.invaderPlus; i ++) {
            int posx = 0;
            srand(time(NULL)); //random seed
            if (!invaders[i].alive) {
                invaders[i].alive = 1;
                bool found = false;
                while (!found) {
                    posx = ((rand() % 41)+2); // maradekos osztas + x hogy a random szam a kivant intervallumban legyen
                    if (free_space(posx, invaders, fromPlayer.invaderPlus)) {
                        found = true;
                    }
                }
                invaders[i].x = posx;
                invaders[i].y = -2 - (rand() % 10); // kezdo pozicio
            }
        }
        // SET METEORS IF NEEDED
        if (fromPlayer.isMeteor) {
            for (int i = 0; i < MAX_M; i++) {
                int posxx = 0;
                srand(time(NULL)); // random seed
                if (meteors[i].active == 0) {
                    meteors[i].active = 1;
                    bool found = false;
                    while (!found) {
                        posxx = (rand() % 41) + 2;
                        if(free_space_meteor(posxx, meteors)) found = true;
                    }
                    meteors[i].x = posxx;
                    meteors[i].y = 0 - (rand()%15);
                }
            }
        }

        // invaderek frissitese, majd print --> bot priority
        run2 = step_invader(win, invaders, &stp_invader, fromPlayer.invaderPlus, heady, headx);
        
        //meteor frissites
        if (fromPlayer.isMeteor) {
            run = step_meteor(win, meteors, &stp_meteor, headx);
        }

        //hatar megrajzolasa az invaderek rajzolasa utan!!!
        box(win, 0, 0);

        // projectile frissites, majd print
        for (int i = 0; i < MAX_PROJ; i ++) {
            if (projectiles[i].active) {
                mvwaddch(win, projectiles[i].y, projectiles[i].x, '|');
                projectiles[i].y --;
                if (projectiles[i].y < 1) {
                    projectiles[i].active = 0; //turn off
                }
            }
        }

        // check collision with enemy and meteors
        for (int i = 0; i < MAX_PROJ; i++){
            if (projectiles[i].active) {
                for (int j = 0; j < MAX_INV+fromPlayer.invaderPlus; j++) {
                    if (collision(i, j, projectiles, invaders)) {
                        
                        // score noveles
                        points = points + (50 - invaders[j].y);
                        invaders[j].alive = 0;
                        projectiles[i].active = 0;
                        
                        break;
                        
                    }
                    if (fromPlayer.isMeteor) {
                        if (collision_meteor(i, j, projectiles, meteors)) {
                            meteors[j].hp = meteors[j].hp - 1;
                            projectiles[i].active = 0;
                            if (meteors[j].hp == 0) {
                                // reset meteor and deactivate for respawn
                                meteors[j].active = 0;
                                meteors[j].hp = 2;
                            }
                            break;
                        }

                    }
                }
            }
        }

        // Gettink keys and rest
        int ch = wgetch(win);

        if (ch == KEY_RIGHT && headx < 46) {
            headx = headx + 1;
        } else if (ch == KEY_LEFT && headx > 4) {
            headx = headx - 1;
        } else if (ch == ' ') {
            //firing
            for (int i = 0; i < MAX_PROJ; i++) {
                if (!projectiles[i].active) {
                    projectiles[i].x = headx;
                    projectiles[i].y = heady - 1;
                    projectiles[i].active = 1;
                    break;
                }
            }
        } else if (ch == '\n') {
            nodelay(win, false);
            int ch1;
            mvwprintw(win,15, 8, "GAME STOPPED, hit SPACE to continue");
            wrefresh(win);
            ch1 = wgetch(win);
            if (ch1 == ' ') {
                run = 1;
            } else {
                run = 0;
                clear();

            }
            nodelay(win, true);
        }
        int xxx = 3;
        //mid_hor(score, &xxx, 3);
        mvwprintw(score, 5, xxx, "%d", points);
        //mid_hor(score, &xxx, strlen(fromPlayer.name));
        mvwprintw(score, 3, xxx, "%s", fromPlayer.name);
        wrefresh(score);
        // wrefresh(win);
        refresh();

        // small hardness mechanic
        if (points > 5000) usleep(10000);
        else usleep(20000-points*2);   
    
    }
    
    playerOut out;
    strncpy(out.name, fromPlayer.name, sizeof(out.name)-1);
    out.name[sizeof(out.name)-1] = '\0';
    out.points = points;
    return out;
}