//
// Created by Sobhan on 1/25/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "Player.h"
#include "Android.h"
#include "Mechanism.h"
#define MAX_SAVE_SIZE 5000

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
        //printf("Ship Hit :)\n");
        brd->square[y][x].is_visible = true;
        *brd->square[y][x].health = false;
        //check for other possible changes
        return 1;
    }
    printf("You hit the Water :(\n");
    brd->square[y][x].is_visible = 1;
    return 0;
}

int play_player(struct player *offensive_pl, struct board *defensive_brd)
{
    //disp_player_fast(offensive_pl, false);
    printf("%s Has %d  Points in Current Game\n", offensive_pl->iden->name, offensive_pl->points);
    int x, y, res, sink_points;
    do{
        printf("Turn Map:\n");
        disp_board_fast(defensive_brd, 0);
        printf("Enter row and column: ");
        scanf("%d%d", &y, &x);
        res = hit(defensive_brd, x, y);
    } while(res == -1);

    if(res == 1) {
        printf("You Hit a ship :)\n");
        offensive_pl->points++;
        sink_points = check_afloat_ships(defensive_brd);
        if(sink_points) {
            printf("You Sunk a ship ;)\n");
            offensive_pl->points += sink_points;
            printf("Result Map:\n");
            disp_board_fast(defensive_brd, 0);
            if(defensive_brd->afloat_ships->next == NULL){
                printf("You Won :D\n");
                make_board_visible(defensive_brd);
                disp_board_fast(defensive_brd, 0);
                if(save_identity(offensive_pl, 1))
                    printf("Winner Results Saved\n");
                return 0;
            } else{
                printf("Result Map:\n");
                disp_board_fast(defensive_brd, 0);
            }
        }
        return play_player(offensive_pl, defensive_brd);
    } else {
        printf("Result Map:\n");
        disp_board_fast(defensive_brd, 0);
    }
    if(res == -2)
        return -1;
    return 1;
}

int play_android(struct android *bot, struct board *brd)
{
    //disp_android_fast(bot, false);
    printf("Bot Has %d  Points in Current Game\n", bot->points);
    int res, sink_points;
    double max_found = 10000;
    struct board_min *brd_min;
    do {
        struct location loc = get_hit_loc(brd, 5, max_found);
        res = hit(brd, loc.x, loc.y);
    } while(res == -1);

    if(res == 1){
        printf("Bot Hit a ship :|\n");
        //max_found /= 1.1;
        bot->points++;
        sink_points = check_afloat_ships(brd);
        if(sink_points){
            printf("Bot Sunk a ship :/\n");
            bot->points += sink_points;
            printf("Result Map:\n");
            disp_board_fast(brd, 0);
            if(brd->afloat_ships->next == NULL){
                //max_found = 10000;
                printf("Bot Won :p\n");
                make_board_visible(brd);
                disp_board_fast(brd, 0);
                return 0;
            }
        } else {
            printf("Result Map:\n");
            disp_board_fast(brd, 0);
        }
        return play_android(bot, brd);
    } else {
        printf("Result Map:\n");
        disp_board_fast(brd, 0);
    }
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

bool save_enactor(void *en, bool is_bot, FILE *fout)
{
    if(is_bot)
        return write_bot2file((struct android*)en, fout);
    return write_player2file((struct player*)en, fout);
}

void* load_enactor(bool is_bot, int *points, FILE *fin)
{
    printf("load_enactor\n");
    if(is_bot)
        return read_bot_from_file(points, fin);
    return read_player_from_file(points, fin);
}

bool save_game(struct player *offensive_pl, void *defensive, bool is_pvp)
{
    if(is_pvp){
        struct player *defensive_pl = (struct player*)defensive;
    } else{
        struct bot *defensive_bot = (struct bot*)defensive;
    }
    int save_id = get_first_free_int();
    FILE *meta_fout = fopen("../resources/saves/meta", "r+b");
    FILE *fout = fopen("../resources/saves/saves", "r+b");
    if(fout == NULL || meta_fout == NULL){
        meta_fout = fopen("../resources/saves/meta", "wb");
        fout = fopen("../resources/saves/saves", "wb");
        fclose(meta_fout);
        fclose(fout);
        meta_fout = fopen("../resources/saves/meta", "r+b");
        fout = fopen("../resources/saves/saves", "r+b");
        if(fout == NULL || meta_fout == NULL){
            printf("Could Not Open File\n");
            return false;
        }
    }
    fseek(meta_fout, 0, SEEK_END);
    fseek(fout, 0, SEEK_END);
    long beg_loc = ftell(fout), end_loc;
    struct meta *met = (struct meta*)malloc(sizeof(struct meta));
    met->ID = save_id;
    met->is_pvp = is_pvp;
    printf("Enter save name: (Default: Save %d)\n", save_id);
    fflush(stdin);
    gets(met->save_name);
    if(met->save_name[0] == 0)
        sprintf(met->save_name, "Save %d", save_id);
    if(save_enactor(offensive_pl, false, fout) && save_enactor(defensive, !is_pvp, fout)){
        end_loc = ftell(fout);
        met->size = end_loc - beg_loc;
        if(met->size > MAX_SAVE_SIZE)
            printf("MAX_SAVE_SIZE EXCEEDED\n");
        if(fwrite(met, sizeof(struct meta), 1, meta_fout)) {
            fclose(fout);
            fclose(meta_fout);
            free(met);
            return true;
        } else
            printf("Could Not Save Meta\n");
    } else
        printf("Could Not Save The Game\n");
    fclose(fout);
    fclose(meta_fout);
    free(met);
    return false;
}

bool show_saves(bool verbose)
{
    FILE *fin = fopen("../resources/saves/meta", "rb");
    if(fin == NULL){
        FILE  *fout = fopen("../resources/saves/meta", "wb");
        fclose(fout);
        fin = fopen("../resources/saves/meta", "rb");
        if(fin == NULL){
            printf("Could Not Load Saves Meta File\n");
            return false;
        }
        fclose(fin);
        printf("There are no saves\n");
        return false;
    }
    bool not_empty = false;
    struct meta *met = (struct meta*)malloc(sizeof(struct meta));
    while(fread(met, sizeof(struct meta), 1, fin)){
        printf("%s", met->save_name);
        not_empty = true;
        if(verbose) {
            if(met->is_pvp)
                printf("\tPVP");
            else
                printf("\tSingle Player");
            printf("\t%d\t%ld", met->ID, met->size);
        }
        printf("\n");
    }
    free(met);
    fclose(fin);
    if(!not_empty)
        printf("There are no saves\n");
    return not_empty;
}

bool load_game()
{
    //printf("Game Saves:\n");
    if(!show_saves(false))
        return false;
    char name[NAME_LEN];
    printf("Enter Name to Load the Saved Game:\n");
    fflush(stdin);
    gets(name);
    rename("../resources/saves/saves", "../resources/saves/0_saves");
    rename("../resources/saves/meta",  "../resources/saves/0_meta");
    FILE *fin_meta = fopen("../resources/saves/0_meta", "rb");
    FILE *fin = fopen("../resources/saves/0_saves", "rb");
    FILE *fout_meta = fopen("../resources/saves/meta", "wb");
    FILE *fout = fopen("../resources/saves/saves", "wb");
    if(fin == NULL || fin_meta == NULL ){
        if(fout == NULL || fout_meta == NULL){
            printf("Could Not Open The Save File\n");
            return false;
        }
        printf("There are no saved games. (weird)\n");
        fclose(fout_meta);
        fclose(fout);
        remove("../resources/saves/0_saves");
        remove("../resources/saves/0_meta");
        return true;
    }
    //printf("loaded correctly\n");
    struct meta *met = (struct meta*)malloc(sizeof(struct meta));
    char *temp_str = (char*)malloc(MAX_SAVE_SIZE * sizeof(char));
    bool found = false;
    while(fread(met, sizeof(struct meta), 1, fin_meta)){
        if(strcmp(met->save_name, name) == 0) {
            found = true;
            break;
        }
        if(fwrite(met, sizeof(struct meta), 1, fout_meta) == 0 ||
                fread(temp_str, sizeof(char), met->size, fin) < met->size ||
                fwrite(temp_str, sizeof(char), met->size, fout) < met->size) {
            printf("Could not transfer the Save Files\n");
            fclose(fin);
            fclose(fin_meta);
            fclose(fout);
            fclose(fout_meta);
            remove("../resources/saves/saves");
            remove("../resources/saves/meta");
            rename("../resources/saves/0_saves", "../resources/saves/saves");
            rename("../resources/saves/0_meta",  "../resources/saves/meta");
            return false;
        }
    }
    if(!found){
        printf("A saved game with entered named does not exist\n");
        fclose(fin);
        fclose(fin_meta);
        fclose(fout);
        fclose(fout_meta);
        remove("../resources/saves/0_saves");
        remove("../resources/saves/0_meta");
        return true;
    }
    int points_pl1 = 0, points_pl2 = 0;
    struct player *pl1 = load_enactor(false, &points_pl2, fin);
    struct player *pl2;
    struct android *bot;
    if(met->is_pvp)
        pl2 = load_enactor(!met->is_pvp, &points_pl1, fin);
    else
        bot = load_enactor(!met->is_pvp, &points_pl1, fin);
    if(pl1 == NULL || pl2 == NULL){
        printf("Could Not Load Players\n");
        destroy_player(pl1);
        destroy_player(pl2);
        fclose(fin);
        fclose(fin_meta);
        fclose(fout);
        fclose(fout_meta);
        remove("../resources/saves/saves");
        remove("../resources/saves/meta");
        rename("../resources/saves/0_saves", "../resources/saves/saves");
        rename("../resources/saves/0_meta",  "../resources/saves/meta");
        return false;
    }
    pl1->points = points_pl1;
    if(met->is_pvp)
        pl2->points = points_pl2;
    else
        bot->points = points_pl2;

    while(fread(met, sizeof(struct meta), 1, fin_meta)){
        if(fwrite(met, sizeof(struct meta), 1, fout_meta) == 0 ||
           fread(temp_str, sizeof(char), met->size, fin) < met->size ||
           fwrite(temp_str, sizeof(char), met->size, fout) < met->size) {
            printf("Could not transfer the Save Files\n");
            fclose(fin);
            fclose(fin_meta);
            fclose(fout);
            fclose(fout_meta);
            remove("../resources/saves/saves");
            remove("../resources/saves/meta");
            rename("../resources/saves/0_saves", "../resources/saves/saves");
            rename("../resources/saves/0_meta",  "../resources/saves/meta");
            return false;
        }
    }
    fclose(fin);
    fclose(fin_meta);
    fclose(fout);
    fclose(fout_meta);
    remove("../resources/saves/0_saves");
    remove("../resources/saves/0_meta");
    if(met->is_pvp)
        run_game_pvp(pl1, pl2);
    else
        run_game_pvb(pl2, bot);
    return true;
}

bool init_game_pvp()
{
    struct player *pl1 = init_player(-1), *pl2 = init_player(pl1->iden->ID);
    if(pl1 == NULL || pl2 == NULL) {
        free(pl1);
        free(pl2);
        return false;
    }
    run_game_pvp(pl1, pl2);
    return true;
}

void run_game_pvp(struct player *pl1, struct player *pl2)
{
    disp_player_fast(pl1, true);
    disp_player_fast(pl2, true);
    struct player *offensive_pl, *defensive_pl, *tmp_pl;
    offensive_pl = pl2;
    defensive_pl = pl1;
    int state; //-1: exit game, 0: game has ended, 1: change turn
    do {
        tmp_pl = offensive_pl;
        offensive_pl = defensive_pl;
        defensive_pl = tmp_pl;
        state = play_player(offensive_pl, defensive_pl->brd);
    } while(state == 1);
    if(state == 0){
        if(save_identity(defensive_pl, 0))
            printf("Looser Results Saved\n");
    }
    if(state == -1){
        int choice;
        printf("Do you want to save the game? (No:0, Yes:1) ");
        scanf("%d", &choice);
        if(choice){
            if(save_identity(pl1, -1) && save_identity(pl2, -1) && save_game(offensive_pl, defensive_pl, true))
                printf("Game Saved\n");
            else
                printf("Could Not Save The Game");
        }
    }
    destroy_player(pl1);
    destroy_player(pl2);
}

bool init_game_pvb()
{
    struct player *pl = init_player(-1);
    if(pl == NULL)
        return false;
    struct android *bot = init_bot();
    run_game_pvb(pl, bot);
    return true;
}

void run_game_pvb(struct player *pl, struct android *bot)
{
    disp_player_fast(pl, true);
    disp_android_fast(bot, true);
    bool pl_turn = false;
    int state; //-1: exit game, 0: game has ended, 1: change turn
    do{
        pl_turn = !pl_turn;
        if(pl_turn)
            state = play_player(pl, bot->brd);
        else
            state = play_android(bot, pl->brd);
    } while (state == 1);
    if(state == 0 && !pl_turn && save_identity(pl, 0)){
        printf("Looser Results Saved\n");
    }
    if(state == -1) {
        int choice;
        printf("Do you want to save the game? (No:0, Yes:1) ");
        scanf("%d", &choice);
        if (choice) {
            if (save_identity(pl, -1) && save_game(pl, bot, false))
                printf("Game Saved\n");
            else
                printf("Could Not Save The Game");
        }
    }
    destroy_player(pl);
    destroy_android(bot);
}