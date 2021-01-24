//
// Created by Sobhan on 1/25/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include "Config.h"
#include "Board.h"

struct config_ship_list* init_config_ship_list(struct ship_tmp *ship, int count)
{
    struct config_ship_list *list = (struct config_ship_list*)malloc(sizeof(struct config_ship_list));
    list->next = NULL;
    list->ship = ship;
    list->count = count;
    return list;
}

struct config* get_conf()
{
    int len[] = {5, 3, 2, 1}, wid[] = {1, 1, 1, 1}, points[] = {5, 8, 12, 25}, count[] = {1, 2, 3, 4};
    struct ship_tmp **ships = (struct ship_tmp**)malloc(4 * sizeof(struct ship_tmp*));
    for(int i = 0; i < 4; i++)
        ships[i] = init_ship_tmp(len[i], wid[i], points[i]);

    struct config_ship_list *list_beg = init_config_ship_list(NULL, 0), *cur = list_beg;
    for(int i = 0; i < 4; i++){
        cur->next = init_config_ship_list(ships[i], count[i]);
        cur = cur->next;
    }
    struct config *conf;
    conf = (struct config*)malloc(sizeof(struct config));
    conf->board_size = 10;
    conf->ship_list = list_beg;
    return conf;
}