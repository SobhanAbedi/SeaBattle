//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_MECHANISM_H
#define SEABATTLE_MECHANISM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "Player.h"
#include "Android.h"
#define NAME_LEN 64

//struct declarations
struct meta;

//function declarations
int hit(struct board *brd, int x, int y);
int play_player(struct player *offensive_pl, struct board *defensive_brd, struct player_simp *op_simp, struct player_simp *dp_simp);
int play_android(struct android *bot, struct board *brd, struct player_simp *bot_simp, struct player_simp *pl_simp);
int get_first_free_int();
bool save_enactor(void *en, bool is_bot, FILE *fout);
void* load_enactor(bool is_bot, int *points, FILE *fin);
bool save_game(struct player *offensive_pl, void *defensive, bool is_pvp);
bool show_saves(bool verbose);
int get_saves(char **list);
bool load_game();
bool init_game_pvp();
bool run_game_pvp(struct player *pl1, struct player *pl2);
bool init_game_pvb();
bool run_game_pvb(struct player *pl, struct android *bot);


//struct definitions
struct meta
{
    int ID;
    char save_name[NAME_LEN];
    bool is_pvp;
    long size;
};

#endif //SEABATTLE_MECHANISM_H
