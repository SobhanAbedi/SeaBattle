//
// Created by Sobhan on 1/27/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include "Android.h"
#include "Board.h"

struct android* init_bot()
{
    struct android* bot = (struct android*)malloc(sizeof(struct android));
    bot->points = 0;
    bot->brd = init_board();
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
        destroy_bot(bot);
        bot = NULL;
        return bot;
    }
    bot->points = 0;
    return bot;
}

void destroy_bot(struct android *bot)
{
    destroy_board(bot->brd);
    free(bot);
}