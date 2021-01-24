//
// Created by Sobhan on 1/25/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Player.h"
#include "Config.h"
#include "Board.h"

struct player* init_player()
{
    struct player *pl = (struct player*)malloc(sizeof(struct player));
    strcpy(pl->name, "Sobhan");
    pl->total_games = 1;
    pl->won_games = pl->lost_games = 0;
    pl->brd = init_board();
    return pl;
}