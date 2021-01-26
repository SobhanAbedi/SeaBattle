#include <stdio.h>
#include "Board.h"
#include "Config.h"
#include "Player.h"
#include "Mechanism.h"

int main() {
    //struct config *conf = get_conf();
    //struct board *brd1 = init_board(conf);
    //printf("Looks All Right\n");
    //struct ship_tmp **ships_tmp = get_ship_tmps();
    init_systems();
    struct player *pl1 = init_player(-1), *pl2 = init_player(pl1->iden->ID);
    disp_player_fast(pl1);
    disp_player_fast(pl2);
    run_game_pvp(pl1, pl2);
    disp_player_fast(pl1);
    disp_player_fast(pl2);
    return 0;
}
