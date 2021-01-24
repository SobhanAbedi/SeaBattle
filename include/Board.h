//
// Created by Sobhan on 1/23/2021.
//

#ifndef SEABATTLE_BOARD_H
#define SEABATTLE_BOARD_H

#include <stdbool.h>

//structs declaration
struct ship_tmp;
struct location;
struct location_ext;
struct ship_list;
struct house;
struct board;
struct config_ship_list;
struct config;

//functions declaration
struct ship_tmp* init_ship_tmp(int len, int wid, int points);
struct config_ship_list* init_config_ship_list(struct ship_tmp *ship, int count);
struct ship_list* init_ship_list(struct ship_tmp *ship, struct location *ploc);
struct config* get_conf();
struct location_ext* get_location_ext(struct location *loc);
bool can_place_ship(struct house** square, int board_size, struct ship_tmp *ship, struct location_ext *loc);
bool place_ship(struct house** square, int board_size, struct ship_list *ship);
struct board* init_board(struct config *conf);
bool destroy_ship_list(struct ship_list *ship);
bool destroy_board(struct board *brd);

//structs definition
struct ship_tmp{
    int len, wid, points;
};

struct location{
    int x, y, dir;
};

struct location_ext{
    int x, y, dx_wid, dy_wid, dx_len, dy_len;
};

struct ship_list{
    struct ship_list *next;
    struct ship_tmp *ship;
    struct location loc;
    bool **health, is_afloat;
};

struct house{
    bool is_ship, is_visible;
    bool *health;
};

struct board{
    struct ship_list *afloat_ships, *sunken_ships;
    int size;
    struct house **square;
};

struct config_ship_list{
    struct config_ship_list *next;
    struct ship_tmp *ship;
    int count;
};

struct config{
    int board_size;
    struct config_ship_list *ship_list;
};

#endif //SEABATTLE_BOARD_H
