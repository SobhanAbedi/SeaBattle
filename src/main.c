#include <stdio.h>
#include <stdlib.h>
#include "Graphics.h"
#include "Board.h"
#include "Config.h"
#include "Player.h"
#include "Android.h"
#include "Mechanism.h"

int main(int argv, char *argc[]) {
    bool is_running = true;
    init_systems();
    SDL_Delay(1000);

    while(is_running){
        if(!check_window())
            is_running = false;
    }

    close_systems();
    return 0;

    struct board *brd = init_board();
    printf("%d\n", can_fill_board(brd, 1000));


    disp_top5_players();

    printf("Do you want to load a game? (No:0, Yes:1) ");
    int choice;
    scanf("%d", &choice);
    if(choice == 1)
        load_game();
    else {
        printf("PVB: 0, PVP: 1");
        scanf("%d", &choice);
        if(choice)
            init_game_pvp();
        else
            init_game_pvb();
    }
    close_systems();
    disp_top5_players();
    show_saves(true);
     return 0;
}
