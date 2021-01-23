//
// Created by Sobhan on 1/23/2021.
//

#ifndef SEABATTLE_BOARD_H
#define SEABATTLE_BOARD_H

#include <stdbool.h>

struct ship_tmp;
struct location;
struct ship_list;
struct board;
struct config_ship_list;
struct config;

struct ship_tmp{
    int len, wid, points;
};

struct location{
    int x, y, dir;
};

struct ship_list{
    struct ship_list *next;
    struct ship_tmp *ship;
    struct location *loc;
    bool *health, is_afloat;
};

struct board{
    struct ship_list *afloat_ships, *sunken_ships;
    int size;
    int *state;
};

struct config_ship_list{
    struct config_ship_list *next;
    struct ship_tmp *ship;
    int coutn;
};

struct config{
    int board_size;
    struct config_ship_list *ship_list;
};

#endif //SEABATTLE_BOARD_H
