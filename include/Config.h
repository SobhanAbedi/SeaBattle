//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_CONFIG_H
#define SEABATTLE_CONFIG_H

#define SHIP_COUNT 8

#include "Board.h"

//structs declaration
struct config_ship_list;
struct config;

//functions declaration
void init_systems();
struct config_ship_list* new_config_ship_list_ent(struct ship_tmp *ship, int count);
void destroy_config_ship_list(struct config_ship_list *list);
void restore_conf();
struct config* get_conf();

//structs definition
struct config_ship_list{
    struct config_ship_list *next;
    struct ship_tmp *ship;
    int count;
};

struct config{
    int board_size;
    struct config_ship_list *ship_list;
};

#endif //SEABATTLE_CONFIG_H
