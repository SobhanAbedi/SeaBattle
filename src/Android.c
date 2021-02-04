//
// Created by Sobhan on 1/27/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include "Android.h"

struct android* init_bot()
{
    struct android* bot = (struct android*)malloc(sizeof(struct android));
    bot->points = 0;
    bot->brd = init_board();
    return bot;
}

bool write_bot2file(struct android *bot, FILE *fout)
{
    return write_board2file(bot->brd, fout);
}

struct android* read_bot_from_file(int *points, FILE *fin)
{
    struct android *bot = (struct android*)malloc(sizeof(struct android));
    bot->brd = read_board_from_file(points, fin);
    if(bot->brd == NULL){
        destroy_android(bot);
        bot = NULL;
        return bot;
    }
    bot->points = 0;
    return bot;
}

struct location get_hit_loc(struct board_min *main_brd)
{
    bool found = false;
    struct location loc;
    loc.dir = loc.x = loc.y = -1;
    for(int i = 0; !found && i < main_brd->size; i++)
        for(int j = 0; !found && j < main_brd->size; j++)
            if(main_brd->square[i][j] == -1){
                loc.x = j;
                loc.y = i;
                found = true;
            }
    return loc;
}

void destroy_android(struct android *bot)
{
    destroy_board(bot->brd);
    free(bot);
}