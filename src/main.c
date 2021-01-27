#include <stdio.h>
#include "Board.h"
#include "Config.h"
#include "Player.h"
#include "Mechanism.h"

int main() {
    init_systems();
    disp_top5_players();

    printf("Do you want to load a game? (No:0, Yes:1) ");
    int choice;
    scanf("%d", &choice);
    if(choice == 1)
        load_game();
    else
        init_game_pvp();
    close_systems();
    disp_top5_players();
    show_saves(true);
     return 0;
}
