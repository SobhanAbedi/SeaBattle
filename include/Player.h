//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_PLAYER_H
#define SEABATTLE_PLAYER_H

#include "Board.h"
#include "Config.h"
#define NAME_LEN 64
//structs declaration
struct player;

//functions
struct player* init_player();

//structs definition
struct player
{
    char name[NAME_LEN];
    int total_games, won_games, lost_games;
    //int open_games = total_games - won_games - lost_games;
    struct board *brd;
};

#endif //SEABATTLE_PLAYER_H
