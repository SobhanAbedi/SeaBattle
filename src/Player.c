//
// Created by Sobhan on 1/25/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Player.h"
#include "Board.h"

struct identity* get_player(int other_player_ID)
{
    struct identity *iden = (struct identity*)malloc(sizeof(struct identity));
    iden->ID = 1;
    if(iden->ID == other_player_ID)
        return NULL;
    strcpy(iden->name, "Sobhan");
    iden->lost_games = iden->won_games = 0;
    iden->total_games = 0 + 1;
    iden->points = 0;
    return iden;
}

void disp_identity_fast(struct identity *iden)
{
    printf("%s : %d Points\nTotal Games: %d\tGames Won: %d\tGames Lost: %d\tOpen Games: %d\n", iden->name, iden->points, iden->total_games, iden->won_games, iden->lost_games, iden->total_games - iden->won_games - iden->lost_games);
}

struct player* init_player()
{
    struct player *pl = (struct player*)malloc(sizeof(struct player));
    pl->iden = get_player(-1);
    pl->brd = init_board();
    pl->points = 0;
    return pl;
}

void disp_player_fast(struct player *pl)
{
    disp_identity_fast(pl->iden);
    printf("Current Game: %d Points\n", pl->points);
    disp_board_fast(pl->brd, 1);
}