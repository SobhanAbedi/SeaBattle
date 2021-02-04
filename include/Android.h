//
// Created by Sobhan on 1/27/2021.
//

#ifndef SEABATTLE_ANDROID_H
#define SEABATTLE_ANDROID_H

#include <stdbool.h>
#include "Board.h"

//struct declarations
struct android;

//function declarations
struct android* init_bot();
bool write_bot2file(struct android *bot, FILE *fout);
struct android* read_bot_from_file(int *points, FILE *fin);
struct location get_hit_loc(struct board_min *main_brd);
void destroy_android(struct android *bot);

//struct definitions
struct android
{
    struct board *brd;
    int points;
};

#endif //SEABATTLE_ANDROID_H
