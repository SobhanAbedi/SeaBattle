//
// Created by Sobhan on 1/23/2021.
//

#ifndef SEABATTLE_BOARD_H
#define SEABATTLE_BOARD_H

#include <stdbool.h>
#include "Config.h"

//structs declaration
struct ship_tmp;
struct location;
struct location_ext;
struct ship_list;
struct house;
struct board;

//functions declaration
struct ship_tmp new_ship_tmp(int len, int wid, int points);
struct ship_list* new_ship_list_ent(struct ship_tmp *ship, struct location *ploc);
struct location_ext* get_location_ext(struct location *loc);
bool can_place_ship(struct house** square, int board_size, struct ship_tmp *ship, struct location_ext *loc);
bool place_ship(struct house** square, int board_size, struct ship_list *ship);
void make_visible_around_sunken_ship(struct board *brd, struct ship_list *ent);
int check_afloat_ships(struct board *brd);
struct ship_list* copy_ship_list(struct ship_list *list1);
bool rem_ship_list_ent(struct ship_list *pre);
struct board* init_board();
bool write_ship_list2file(struct ship_list *list, FILE *fout);
bool write_house2file(struct board *brd, FILE *fout);
bool write_board2file(struct board *brd, FILE *fout);
void disp_board_fast(struct board *brd, bool debug);
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
    bool *is_afloat, *health;
};

struct board{
    struct ship_list *afloat_ships, *sunken_ships;
    int size;
    struct house **square;
};

#endif //SEABATTLE_BOARD_H
