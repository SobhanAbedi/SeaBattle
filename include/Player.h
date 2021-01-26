//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_PLAYER_H
#define SEABATTLE_PLAYER_H

#include "Board.h"
#include "Config.h"
#define NAME_LEN 64
//structs declaration
struct identity;
struct player;

//functions declaration
struct identity* get_player(int other_player_ID);
bool save_player(struct player *pl, bool has_won);
void disp_identity_fast(struct identity *iden);
struct player* init_player(int other_player_ID);
void disp_player_fast(struct player *pl);

//structs definition

struct identity
{
    char name[NAME_LEN];
    int ID;
    int total_games, won_games, lost_games;
    //int open_games = total_games - won_games - lost_games;
    int points;
};

struct player
{
    struct identity *iden;
    struct board *brd;
    int points;
};

#endif //SEABATTLE_PLAYER_H
