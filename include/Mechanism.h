//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_MECHANISM_H
#define SEABATTLE_MECHANISM_H

//structs declaration

//functions declaration
int hit(struct board *brd, int x, int y);
bool play_player(struct player *offensive_pl, struct player *defensive_pl );
void run_game_pvp(struct player *pl1, struct player *pl2);
//structs definition


#endif //SEABATTLE_MECHANISM_H
