//
// Created by Sobhan on 1/27/2021.
//

#ifndef SEABATTLE_ANDROID_H
#define SEABATTLE_ANDROID_H

#include <stdbool.h>

//struct declarations
struct android;

//function declarations
struct android* init_bot();
bool write_bot2file(struct android *bot, FILE *fout);
struct android* read_bot_from_file(int *points, FILE *fin);
void destroy_bot(struct android *bot);

//struct definitions
struct android
{
    struct board *brd;
    int points;
};

#endif //SEABATTLE_ANDROID_H
