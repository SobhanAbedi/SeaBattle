//
// Created by Sobhan on 1/25/2021.
//

#include <stdio.h>
#include "Board.h"
#include "Player.h"
#include "Mechanism.h"



int hit(struct board *brd, int x, int y)
{
    if(brd->square[y][x].is_visible){
        printf("You Cant Hit an Already Visible Square\n");
        return -1;
    }
    if(brd->square[y][x].is_ship){
        printf("You hit a Ship :)\n");
        brd->square[y][x].is_visible = true;
        *brd->square[y][x].health = false;
        //check for other possible changes
        return 1;
    }
    printf("You hit the Water :(\n");
    brd->square[y][x].is_visible = 1;
    return 0;
}

bool play_player(struct player *offensive_pl, struct player *defensive_pl )
{
    int x, y, res, sink_points;
    do{
        printf("Turn Map:\n");
        disp_board_fast(defensive_pl->brd, 0);
        printf("Enter row and column: ");
        scanf("%d%d", &y, &x);
        res = hit(defensive_pl->brd, x, y);
    } while(res == -1);
    if(res == 1) {
        offensive_pl->points++;
        sink_points = check_afloat_ships(defensive_pl->brd);
        if(sink_points) {
            printf("You Suck a ship ;)\n");
            offensive_pl->points += sink_points;
            if(defensive_pl->brd->afloat_ships->next == NULL){
                printf("You Won :D\n");
                if(save_player(offensive_pl, 1) && save_player(defensive_pl, 0))
                    printf("Players Results Saved\n");
                return true;
            }
        }
        return play_player(offensive_pl, defensive_pl);
    }
    return false;
}

void run_game_pvp(struct player *pl1, struct player *pl2)
{
    struct player *offensive_pl, *defensive_pl, *tmp_pl;
    offensive_pl = pl1;
    defensive_pl = pl2;
    while (!play_player(offensive_pl, defensive_pl)){
        tmp_pl = offensive_pl;
        offensive_pl = defensive_pl;
        defensive_pl = tmp_pl;
    }

}