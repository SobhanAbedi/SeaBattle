//
// Created by Sobhan on 1/23/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include "Board.h"

struct config* get_conf()
{
    int *len = {5, 3, 2, 1}, *wid = {1, 1, 1, 1}, *points = {5, 8, 12, 25}, *count = {1, 2, 3, 4};
    struct ship_tmp **ships = (struct ship_list**)malloc(4 * sizeof(struct ship_tmp));
    for(int i = 0; i < 4; i++){
        ships[i]->len = len[i];
        ships[i]->wid = wid[i];
        ships[i]->points = points[i];
    }
    struct config_ship_list *cur, *list_beg = (struct config_ship_list*)malloc(sizeof(struct config_ship_list));
    list_beg->coutn = 0;
    list_beg->ship = NULL;
    list_beg->next = NULL;
    cur = list_beg;
    for(int i = 0; i < 4; i++){
        cur->next = (struct config_ship_list*)malloc(sizeof(struct config_ship_list));
        cur = cur->next;
        cur->coutn = count[i];
        cur->ship = ships[i];
        cur->next = NULL;
    }
    struct config *conf;
    conf = (struct config*)malloc(sizeof(struct config));
    conf->board_size = 10;
    conf->ship_list = list_beg;
    return conf;
}