#include <stdio.h>
#include "Board.h"
#include "Config.h"
#include "Player.h"
#include "Mechanism.h"

int main() {
    //struct config *conf = get_conf();
    //struct board *brd1 = init_board(conf);
    //printf("Looks All Right\n");
    struct player *pl1 = init_player(), *pl2 = init_player();
    disp_player_fast(pl1);
    disp_player_fast(pl2);
    run_game_pvp(pl1, pl2);
    disp_player_fast(pl1);
    disp_player_fast(pl2);
    return 0;
}
