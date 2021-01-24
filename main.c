#include <stdio.h>
#include "Board.h"
#include "Config.h"
#include "Player.h"

int main() {
    //struct config *conf = get_conf();
    //struct board *brd1 = init_board(conf);
    //printf("Looks All Right\n");
    struct player *pl1 = init_player();
    disp_board_fast(pl1->brd);
    return 0;
}
