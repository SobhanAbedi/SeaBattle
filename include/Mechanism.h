//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_MECHANISM_H
#define SEABATTLE_MECHANISM_H

//structs declaration
struct meta;

//functions declaration
int hit(struct board *brd, int x, int y);
int play_player(struct player *offensive_pl, struct player *defensive_pl );
void run_game_pvp(struct player *pl1, struct player *pl2);

//structs definition
struct meta
{
    int ID;
    char save_name[NAME_LEN];
    int size;
};

#endif //SEABATTLE_MECHANISM_H
