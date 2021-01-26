//
// Created by Sobhan on 1/25/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "Player.h"
#include "Mechanism.h"



int hit(struct board *brd, int x, int y)
{
    if(x == -1 && y == -1)
        return -2;
    if(x < 0 || y < 0 || x >= brd->size || y >= brd->size){
        printf("Enter a Valid Address\n");
        return -1;
    }
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

int play_player(struct player *offensive_pl, struct player *defensive_pl )
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
                if(save_identity(offensive_pl, 1) && save_identity(defensive_pl, 0))
                    printf("Players Results Saved\n");
                return 0;
            }
        }
        return play_player(offensive_pl, defensive_pl);
    }
    if(res == -2)
        return -1;
    return 1;
}

int get_first_free_int()
{
    int LUI = 0; //Last Used Integer
    FILE *fin = fopen("../resources/saves/meta", "r+b");
    if(fin == NULL){
        fin = fopen("../resources/saves/meta", "wb");
        fclose(fin);
        if(fin == NULL){
            printf("Could Not Open Meta File\n");
            return -1;
        }
        return LUI + 1;
    }
    bool skipped, recheck = 1;
    struct meta *met = (struct meta*)malloc(sizeof(struct meta));
    while(recheck) {
        recheck = false;
        skipped = false;
        while (fread(met, sizeof(struct meta), 1, fin)) {
            if(met->ID > LUI){
                if(met->ID == LUI + 1){
                    LUI++;
                    if(skipped)
                        recheck = true;
                }else
                    skipped = true;
            }
        }
    }
    free(met);
    return LUI + 1;
}

bool save_game(struct player *offensive_pl, struct player *defensive_pl)
{
    FILE *meta_fout = fopen("../resources/saves/meta", "ab");
    FILE *fout = fopen("../resources/saves/saves", "ab");
    if(fout == NULL || meta_fout == NULL) {
        printf("Could Not Open File\n");
        return false;
    }
    int save_id = get_first_free_int();
    long beg_loc = ftell(fout), end_loc;
    struct meta *met = (struct meta*)malloc(sizeof(struct meta));
    met->ID = save_id;
    printf("Enter save name: (Default: Save %d)\n", save_id);
    fflush(stdin);
    gets(met->save_name);
    if(met->save_name[0] == 0)
        sprintf(met->save_name, "Save %d", save_id);
    if(write_player2file(defensive_pl, fout) && write_player2file(offensive_pl, fout)){
        end_loc = ftell(fout);
        met->size = end_loc - beg_loc;
        if(fwrite(met, sizeof(struct meta), 1, meta_fout)) {
            fclose(fout);
            fclose(meta_fout);
            free(met);
            return true;
        } else
            printf("Could Not Save Meta\n");
    }
    fclose(fout);
    fclose(meta_fout);
    free(met);
    return false;
}

void run_game_pvp(struct player *pl1, struct player *pl2)
{
    struct player *offensive_pl, *defensive_pl, *tmp_pl;
    offensive_pl = pl2;
    defensive_pl = pl1;
    int res;
    do {
        tmp_pl = offensive_pl;
        offensive_pl = defensive_pl;
        defensive_pl = tmp_pl;
        res = play_player(offensive_pl, defensive_pl);
    } while(res == 1);
    if(res == -1){
        int choice;
        printf("Do you want to save the game? (No:0, Yes:1) ");
        scanf("%d", &choice);
        if(choice){
            if(save_identity(pl1, -1) && save_identity(pl2, -1) && save_game(offensive_pl, defensive_pl))
                printf("Game Saved\n");
            else
                printf("Could Not Save The Game");
        }
    }

}