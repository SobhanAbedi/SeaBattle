//
// Created by Sobhan on 1/25/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Config.h"
#include "Board.h"
#include "Graphics.h"

//File Specific Global Variable
struct ship_tmp *ship_tmps;

//function definitions
void init_ship_tmps()
{
    static bool is_first = true;
    if(is_first) {
        int len[] = {7, 6, 5, 4, 4, 3, 2, 1}, wid[] = {2, 1, 1, 1, 1, 1, 1, 1}, points[] = {5, 6, 7, 9, 9, 12, 17, 35};
        ship_tmps = (struct ship_tmp*)malloc(SHIP_COUNT * sizeof(struct ship_tmp));
        for (int i = 0; i < SHIP_COUNT; i++)
            ship_tmps[i] = new_ship_tmp(len[i], wid[i], points[i]);
        is_first = false;
    }
}

void init_systems()
{
    init_ship_tmps();
    srand(time(NULL));
    init_graphics(1280, 720, 0);
}

struct config_ship_list* new_config_ship_list_ent(struct ship_tmp *ship, int count)
{
    struct config_ship_list *list = (struct config_ship_list*)malloc(sizeof(struct config_ship_list));
    list->next = NULL;
    list->ship = ship;
    list->count = count;
    return list;
}

void destroy_config_ship_list(struct config_ship_list *list)
{
    if(list->next != NULL)
        destroy_config_ship_list(list->next);
    list->next = NULL;
    free(list);
}

void save_conf(int size, int *count)
{
    FILE *fout = fopen("../resources/config/conf", "wb");
    if(fout == NULL){
        printf("Could Not Save Config File\n");
        return;
    }
    fwrite(&size, sizeof(int), 1, fout);
    fwrite(count, sizeof(int), SHIP_COUNT, fout);
    fclose(fout);
}

void restore_conf()
{
    int size = 10, count[] = {0, 0, 1, 0, 0, 2, 3, 4};
    save_conf(size, count);
}

struct ship_tmp* get_ship_temps()
{
    return ship_tmps;
}

struct config* get_conf()
{
    int size, count[8];
    FILE *fin = fopen("../resources/config/conf", "rb");
    if(fin == NULL){
        restore_conf();
        fin = fopen("../resources/config/conf", "rb");
        if(fin == NULL) {
            printf("Could Not Open Config File\n");
            return NULL;
        }
    }
    fread(&size, sizeof(int), 1, fin);
    fread(count, sizeof(int), SHIP_COUNT, fin);
    fclose(fin);
    struct config *conf = (struct config*)malloc(sizeof(struct config));
    struct config_ship_list *list_beg = new_config_ship_list_ent(NULL, 0), *cur = list_beg;
    for(int i = 0; i < SHIP_COUNT; i++){
        cur->next = new_config_ship_list_ent(&(ship_tmps[i]), count[i]);
        cur = cur->next;
    }
    conf->board_size = size;
    conf->ship_list = list_beg;
    return conf;
}

void close_systems()
{
    free(ship_tmps);
    close_graphics();
}