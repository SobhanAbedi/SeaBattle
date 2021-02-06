//
// Created by Sobhan on 1/25/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Player.h"
#include "Board.h"

struct identity* get_identity(int other_player_ID)
{
    FILE *fin = fopen("../resources/players/players", "rb");
    if(fin == NULL){
        FILE *fout = fopen("../resources/players/players", "wb");
        fclose(fout);
        fin = fopen("../resources/players/players", "rb");
        if(fin == NULL){
            printf("Could Not Open File\n");
            return NULL;
        }
    }
    int last_ID;
    char name[NAME_LEN];
    printf("Enter Your Name: ");
    fflush(stdin);
    gets(name);
    struct identity *iden = (struct identity*)malloc(sizeof(struct identity));
    while(fread(iden, sizeof(struct identity), 1, fin)){
        if(strcmp(name, iden->name) == 0){
            if(iden->ID == other_player_ID){
                printf("You cant play against yourself\n");
                fclose(fin);
                return get_identity(other_player_ID);
            }
            iden->total_games++;
            disp_identity_fast(iden);
            fclose(fin);
            return iden;
        }
        last_ID = iden->ID;
    }
    fclose(fin);
    printf("Record not found. ");
    FILE *fout = fopen("../resources/players/players", "ab");
    if(fout == NULL){
        printf("Could Not Write to file\n");
        return NULL;
    }
    strcpy(iden->name, name);
    iden->ID =  last_ID + 1;
    iden->lost_games = iden->won_games = 0;
    iden->total_games = 0;
    iden->points = 0;
    fwrite(iden, sizeof(struct identity), 1, fout);
    fclose(fout);
    iden->total_games = 1;
    printf("Created new user.\n");
    disp_identity_fast(iden);
    return iden;
}

struct identity* load_identity(int ID)
{
    FILE *fin = fopen("../resources/players/players", "rb");
    if(fin == NULL){
        FILE *fout = fopen("../resources/players/players", "wb");
        fclose(fout);
        fin = fopen("../resources/players/players", "rb");
        if(fin == NULL){
            printf("Could Not Open File\n");
            return NULL;
        }
        return NULL;
    }
    struct identity *iden = (struct identity*)malloc(sizeof(struct identity));
    while(fread(iden, sizeof(struct identity), 1, fin)){
        if(iden->ID == ID){
            fclose(fin);
            return iden;
        }
    }
    free(iden);
    fclose(fin);
    return NULL;
}

bool save_identity(struct player *pl, int has_won)
{
    FILE *fp = fopen("../resources/players/players", "r+b");
    if(fp == NULL){
        printf("Could not open file to save player\n");
        return false;
    }
    struct identity *tmp_iden = (struct identity*)malloc(sizeof(struct identity));
    while (fread(tmp_iden, sizeof(struct identity), 1, fp)){
        if(tmp_iden->ID == pl->iden->ID){
            fseek(fp, (-1l) * sizeof(struct identity), SEEK_CUR);
            if(has_won == 1){
                pl->iden->points += pl->points;
                pl->iden->won_games++;
            } else if(has_won == 0){
                pl->iden->points += pl->points/2;
                pl->iden->lost_games++;
            }
            fwrite(pl->iden, sizeof(struct identity), 1, fp);
            free(tmp_iden);
            fclose(fp);
            return true;
        }
    }
    free(tmp_iden);
    fclose(fp);
    return false;
}

void disp_identity_fast(struct identity *iden)
{
    printf("%s : %d Points\nTotal Games: %d\tGames Won: %d\tGames Lost: %d\tOpen Games: %d\n", iden->name, iden->points, iden->total_games, iden->won_games, iden->lost_games, iden->total_games - iden->won_games - iden->lost_games);
}

struct player* init_player(int other_player_ID)
{
    struct player *pl = (struct player*)malloc(sizeof(struct player));
    pl->iden = get_identity(other_player_ID);
    pl->brd = init_board();
    pl->points = 0;
    return pl;
}

void disp_player_fast(struct player *pl, bool debug)
{
    if(debug)
        disp_identity_fast(pl->iden);
    printf("%s Has %d  Points in Current Game\n", pl->iden->name, pl->points);
    disp_board_fast(pl->brd, debug);
}

void disp_top5_players()
{
    int n = 5;
    FILE *fin = fopen("../resources/players/players", "rb");
    if(fin == NULL) {
        FILE *fout = fopen("../resources/players/players", "wb");
        fclose(fout);
        fin = fopen("../resources/players/players", "rb");
        if(fin == NULL){
            printf("Could Not Open Players File\n");
            return;
        }
        printf("There are no player yet\n");
        return;
    }
    struct identity *iden_list = (struct identity*)malloc(n * sizeof(struct identity));
    struct identity *iden_read = (struct identity*)malloc(sizeof(struct identity));
    for(int i = 0; i < n; i++){
        iden_list[i].ID = -1;
    }
    while(fread(iden_read, sizeof(struct identity), 1, fin)){
        for(int i = 0; i < n; i++){
            if(iden_read->points > iden_list[i].points || iden_list[i].ID == -1){
                for(int j = n - 1; j > i; j--){
                    iden_list[j] = iden_list[j - 1];
                }
                iden_list[i] = *iden_read;
                break;
            }
        }
    }
    for(int i = 0; i < n && iden_list[i].ID != -1; i++){
        printf("Player %d:\n", i + 1);
        disp_identity_fast(&(iden_list[i]));
    }
    free(iden_list);
    free(iden_read);
}

bool write_player2file(struct player *pl, FILE *fout)
{
    if(fwrite(&(pl->iden->ID), sizeof(int), 1, fout) == 1 && write_board2file(pl->brd, fout))
        return true;
    return false;
}

struct player* read_player_from_file(int *points, FILE *fin)
{
    struct player *pl = (struct player*)malloc(sizeof(struct player));
    int ID;
    if(fread(&ID, sizeof(int), 1, fin) == 0) {
        free(pl);
        return NULL;
    }
    pl->iden = load_identity(ID);
    pl->brd = read_board_from_file(points, fin);
    if(pl->iden == NULL || pl->brd == NULL){
        destroy_player(pl);
        pl = NULL;
        return pl;
    }
    pl->points = 0;
    return pl;
}

void destroy_player(struct player *pl)
{
    if(pl == NULL)
        return;
    free(pl->iden);
    destroy_board(pl->brd);
    free(pl);
}