#include <stdio.h>
#include "Board.h"

int main() {
    struct config *conf = get_conf();
    struct board *brd1 = init_board(conf);
    printf("Looks All Right\n");
    return 0;
}