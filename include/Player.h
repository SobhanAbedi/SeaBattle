//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_PLAYER_H
#define SEABATTLE_PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Board.h"
#include "Player.h"
#include "Config.h"
#define NAME_LEN 64
//struct declarations
struct identity;
struct player;
struct player_simp;

//function declarations
struct identity* get_identity(int other_player_ID);
struct identity* load_identity(int ID);
bool save_identity(struct player *pl, int has_won);
void disp_identity_fast(struct identity *iden);
struct player* init_player(int other_player_ID);
void disp_player_fast(struct player *pl, bool debug);
void disp_top5_players();
char** get_top5_players();
bool write_player2file(struct player *pl, FILE *fout);
struct player* read_player_from_file(int *points, FILE *fin);
void destroy_player(struct player *pl);

//struct definitions
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

struct player_simp
{
    bool is_bot;
    char name[NAME_LEN];
    int points;
};

#endif //SEABATTLE_PLAYER_H
