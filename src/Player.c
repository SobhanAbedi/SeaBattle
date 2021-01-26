//
// Created by Sobhan on 1/25/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Player.h"
#include "Board.h"

struct identity* get_player(int other_player_ID)
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
                return get_player(other_player_ID);
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

bool save_player(struct player *pl, bool has_won)
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
            if(has_won){
                pl->iden->points += pl->points;
                pl->iden->won_games++;
            } else{
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
    pl->iden = get_player(other_player_ID);
    pl->brd = init_board();
    pl->points = 0;
    return pl;
}

void disp_player_fast(struct player *pl)
{
    disp_identity_fast(pl->iden);
    printf("Current Game: %d Points\n", pl->points);
    disp_board_fast(pl->brd, 1);
}