//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_CONFIG_H
#define SEABATTLE_CONFIG_H

#include "Board.h"

//structs declaration
struct config_ship_list;
struct config;

//functions
struct config_ship_list* init_config_ship_list(struct ship_tmp *ship, int count);
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
