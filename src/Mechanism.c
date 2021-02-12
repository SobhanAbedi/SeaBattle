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

int play_player(struct player *offensive_pl, struct board *defensive_brd, struct player_simp *op_simp, struct player_simp *dp_simp)
{
    //disp_player_fast(offensive_pl, false);
    printf("%s Has %d  Points in Current Game\n", offensive_pl->iden->name, offensive_pl->points);
    int x, y, res, sink_points;
    op_simp->points = offensive_pl->points;
    struct event_result loc = draw_board(defensive_brd, op_simp, dp_simp, false);
    res = hit(defensive_brd, loc.x1, loc.x2);
    /*
    do{
        printf("Turn Map:\n");
        disp_board_fast(defensive_brd, 0);
        printf("Enter row and column: ");
        scanf("%d%d", &y, &x);
        res = hit(defensive_brd, x, y);
    } while(res == -1);
    */
    if(res == 1) {
        printf("You Hit a ship :)\n");
        offensive_pl->points++;
        op_simp->points = offensive_pl->points;
        sink_points = check_afloat_ships(defensive_brd);
        if(sink_points) {
            printf("You Sunk a ship ;)\n");
            offensive_pl->points += sink_points;
            op_simp->points = offensive_pl->points;
            printf("Result Map:\n");
            disp_board_fast(defensive_brd, 0);
            draw_board(defensive_brd, op_simp, dp_simp, true);
            if(defensive_brd->afloat_ships->next == NULL){
                printf("You Won :D\n");
                make_board_visible(defensive_brd);
                disp_board_fast(defensive_brd, 0);
                draw_board(defensive_brd, op_simp, dp_simp, true);
                if(save_identity(offensive_pl, 1))
                    printf("Winner Results Saved\n");
                return 0;
            } else{
                printf("Result Map:\n");
                disp_board_fast(defensive_brd, 0);
                draw_board(defensive_brd, op_simp, dp_simp, true);
            }
        }
        return play_player(offensive_pl, defensive_brd, op_simp, dp_simp);
    } else {
        printf("Result Map:\n");
        disp_board_fast(defensive_brd, 0);
        draw_board(defensive_brd, op_simp, dp_simp, true);
    }
    if(res == -2)
        return -1;
    return 1;
}

int play_android(struct android *bot, struct board *brd, struct player_simp *bot_simp, struct player_simp *pl_simp)
{
    //disp_android_fast(bot, false);
    bot_simp->points = bot->points;
    draw_board(brd, bot_simp, pl_simp, false);
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
        bot_simp->points = bot->points;
        sink_points = check_afloat_ships(brd);
        if(sink_points){
            printf("Bot Sunk a ship :/\n");
            bot->points += sink_points;
            bot_simp->points = bot->points;
            printf("Result Map:\n");
            disp_board_fast(brd, 0);
            draw_board(brd, bot_simp, pl_simp, true);
            if(brd->afloat_ships->next == NULL){
                //max_found = 10000;
                printf("Bot Won :p\n");
                make_board_visible(brd);
                disp_board_fast(brd, 0);
                draw_board(brd, bot_simp, pl_simp, true);
                return 0;
            }
        } else {
            printf("Result Map:\n");
            disp_board_fast(brd, 0);
            draw_board(brd, bot_simp, pl_simp, true);
        }
        return play_android(bot, brd, bot_simp, pl_simp);
    } else {
        printf("Result Map:\n");
        disp_board_fast(brd, 0);
        draw_board(brd, bot_simp, pl_simp, true);
    }
    return 1;
}

int get_first_free_int()
{
    int LUI = 0; //Last Used Integer
    FILE *fin = fopen("../resources/saves/meta", "rb");
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
        fseek(fin, SEEK_SET, 0);
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
    fclose(fin);
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
    if(is_bot)
        return read_bot_from_file(points, fin);
    return read_player_from_file(points, fin);
}

bool save_game(struct player *offensive_pl, void *defensive, bool is_pvp)
{
    save_identity(offensive_pl, -1);
    if(is_pvp)
        save_identity((struct player*)defensive, -1);

    int save_id = get_first_free_int();
    char name[NAME_LEN];
    sprintf(name, "Save %d", save_id);
    if(!get_save_name(name))
        return false;
    if(name[0] == 0) {
        _fcloseall();
        printf("didn't save\n");
        return true;
    }
    printf("%s saved\n", name);
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
    strcpy(met->save_name, name);

    /*
    printf("Enter save name: (Default: Save %d)\n", save_id);
    fflush(stdin);
    gets(met->save_name);
    if(met->save_name[0] == 0)
        sprintf(met->save_name, "Save %d", save_id);
    */
    if(save_enactor(offensive_pl, false, fout) && save_enactor(defensive, !is_pvp, fout)){
        end_loc = ftell(fout);
        met->size = end_loc - beg_loc;
        if(met->size > MAX_SAVE_SIZE)
            printf("MAX_SAVE_SIZE EXCEEDED\n");
        if(fwrite(met, sizeof(struct meta), 1, meta_fout)) {
            fclose(fout);
            fclose(meta_fout);
            free(met);
            _fcloseall();
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

int get_saves(char **list)
{
    FILE *fin = fopen("../resources/saves/meta", "rb");
    if(fin == NULL){
        FILE  *fout = fopen("../resources/saves/meta", "wb");
        fclose(fout);
        fin = fopen("../resources/saves/meta", "rb");
        if(fin == NULL){
            printf("Could Not Load Saves Meta File\n");
            return -1;
        }
        fclose(fin);
        printf("There are no saves\n");
        return 0;
    }
    bool not_empty = false;
    struct meta *met = (struct meta*)malloc(sizeof(struct meta));
    int k = 0;
    while(k < 10 && fread(met, sizeof(struct meta), 1, fin)){
        sprintf(list[k],"%s", met->save_name);
        not_empty = true;
        k++;
    }
    free(met);
    fclose(fin);
    if(!not_empty)
        printf("There are no saves\n");
    return k;
}

bool load_game()
{
    //printf("Game Saves:\n");
    /*
    if(!show_saves(false))
        return true;
    char name[NAME_LEN];
    printf("Enter Name to Load the Saved Game:\n");
    fflush(stdin);
    gets(name);
     */

    char name[NAME_LEN];
    if(!get_load_name(name))
        return false;
    if(name[0] == 0){
        printf("There are No Saves\n");
        return true;
    }

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
    struct player *pl2 = NULL;
    struct android *bot = NULL;
    if(met->is_pvp) {
        pl2 = load_enactor(false, &points_pl1, fin);
    } else {
        bot = load_enactor(true, &points_pl1, fin);
    }
    if(pl1 == NULL || (pl2 == NULL && bot == NULL)){
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
    bool run_res;
    if(met->is_pvp)
        run_res = run_game_pvp(pl1, pl2);
    else
        run_res = run_game_pvb(pl1, bot);
    free(met);
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
    return run_game_pvp(pl1, pl2);
}

bool run_game_pvp(struct player *pl1, struct player *pl2)
{
    disp_player_fast(pl1, true);
    disp_player_fast(pl2, true);
    struct player *offensive_pl, *defensive_pl, *tmp_pl;
    offensive_pl = pl2;
    defensive_pl = pl1;

    struct player_simp *pl1_simp = (struct player_simp*)malloc(sizeof(struct player_simp));
    strcpy(pl1_simp->name, pl1->iden->name);
    pl1_simp->points = pl1->points;
    pl1_simp->is_bot = false;

    struct player_simp *pl2_simp = (struct player_simp*)malloc(sizeof(struct player_simp));
    strcpy(pl2_simp->name, pl2->iden->name);
    pl2_simp->points = pl2->points;
    pl2_simp->is_bot = false;

    struct player_simp *op_simp, *dp_simp, *tp_simp;
    op_simp = pl2_simp;
    dp_simp = pl1_simp;

    int state; //-1: exit game, 0: game has ended, 1: change turn
    do {
        tmp_pl = offensive_pl;
        offensive_pl = defensive_pl;
        defensive_pl = tmp_pl;

        tp_simp = op_simp;
        op_simp = dp_simp;
        dp_simp = tp_simp;

        state = play_player(offensive_pl, defensive_pl->brd, op_simp, dp_simp);
    } while(state == 1);
    if(state == 0){
        if(save_identity(defensive_pl, 0))
            printf("Looser Results Saved\n");
    }
    bool res;
    if(state == -1){
        res = save_game(offensive_pl, defensive_pl, true);
        /*
        int choice;
        printf("Do you want to save the game? (No:0, Yes:1) ");
        scanf("%d", &choice);
        if(choice){
            if(save_identity(pl1, -1) && save_identity(pl2, -1) && save_game(offensive_pl, defensive_pl, true))
                printf("Game Saved\n");
            else
                printf("Could Not Save The Game");
        }
         */
    }
    destroy_player(pl1);
    destroy_player(pl2);
    return res;
}

bool init_game_pvb()
{
    struct player *pl = init_player(-1);
    if(pl == NULL)
        return false;
    struct android *bot = init_bot();
    return run_game_pvb(pl, bot);
}

bool run_game_pvb(struct player *pl, struct android *bot)
{
    disp_player_fast(pl, true);
    disp_android_fast(bot, true);
    struct player_simp pl_simp;
    strcpy(pl_simp.name, pl->iden->name);
    pl_simp.points = pl->points;
    pl_simp.is_bot = false;

    struct player_simp bot_simp;
    strcpy(bot_simp.name, "Bot");
    bot_simp.points = bot->points;
    bot_simp.is_bot = true;

    bool pl_turn = false;
    int state; //-1: exit game, 0: game has ended, 1: change turn
    do{
        pl_turn = !pl_turn;
        if(pl_turn) {
            state = play_player(pl, bot->brd, &pl_simp, &bot_simp);
            //draw_board(bot->brd, &pl_simp, &bot_simp, 1000);
        } else {
            state = play_android(bot, pl->brd, &bot_simp, &pl_simp);
            //draw_board(pl->brd, &bot_simp, &pl_simp, 1000);
        }
    } while (state == 1);
    if(state == 0 && !pl_turn && save_identity(pl, 0)){
        printf("Your Results Saved\n");
    }
    bool res;
    if(state == -1) {
        res = save_game(pl, bot, false);
        /*
        int choice;
        printf("Do you want to save the game? (No:0, Yes:1) ");
        scanf("%d", &choice);
        if (choice) {
            if (save_identity(pl, -1) && save_game(pl, bot, false))
                printf("Game Saved\n");
            else
                printf("Could Not Save The Game");
        }
         */
    }
    destroy_player(pl);
    destroy_android(bot);
    return res;
}