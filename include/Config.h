//
// Created by Sobhan on 1/25/2021.
//

#ifndef SEABATTLE_CONFIG_H
#define SEABATTLE_CONFIG_H

#define SHIP_COUNT 8

#include "Board.h"

//struct declarations
struct config_ship_list;
struct config;

//function declarations
void init_systems();
struct config_ship_list* new_config_ship_list_ent(struct ship_tmp *ship, int count);
void destroy_config_ship_list(struct config_ship_list *list);
void save_conf(int size, int *count);
void restore_conf();
struct ship_tmp* get_ship_temps();
struct config* get_conf();
void close_systems();

//struct definitions
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
