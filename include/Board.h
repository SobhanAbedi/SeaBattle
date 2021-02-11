//
// Created by Sobhan on 1/23/2021.
//

#ifndef SEABATTLE_BOARD_H
#define SEABATTLE_BOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Config.h"
#include "SDL2/SDL.h"
#include "Graphics.h"


//struct declarations
struct ship_tmp;
struct location;
struct location_ext;
struct ship_list;
struct house;
struct house_min;
struct board;
struct board_min;

//function declarations
struct ship_tmp new_ship_tmp(int len, int wid, int points, struct asset *ast);
struct ship_list* new_ship_list_ent(struct ship_tmp *ship, struct location *ploc);
struct location_ext* get_location_ext(struct location *loc);
bool can_place_ship(struct house** square, int board_size, struct ship_tmp *ship, struct location_ext *loc);
bool place_ship(struct house** square, int board_size, struct ship_list *ship);
void make_visible_around_sunken_ship(struct board *brd, struct ship_list *ent);
int check_afloat_ships(struct board *brd);
struct ship_list* copy_ship_list(struct ship_list *list1);
bool rem_ship_list_ent(struct ship_list *pre);
int fill_board_random(struct board *brd, struct config_ship_list *conf_ship_list);
struct board* init_board();
bool write_ship_list2file(struct ship_list *list, FILE *fout);
struct ship_list* read_ship_list_from_file(struct board *brd, int *points, FILE *fin);
bool write_house2file(struct board *brd, FILE *fout);
struct house** init_house_from_file(int size, FILE *fin);
bool write_board2file(struct board *brd, FILE *fout);
struct board* read_board_from_file(int *points, FILE *fin);
void disp_board_fast(struct board *brd, bool debug);
struct board_min* get_board_for_bot(struct board *brd);
void make_board_visible(struct board *brd);
void destroy_ship_list(struct ship_list *ship);
void destroy_house(struct house** square, int size);
void destroy_board(struct board *brd);
void destroy_board_min(struct board_min *brd, bool is_final);

//struct definitions
struct ship_tmp{
    int len, wid, points;
    struct asset *asset;
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

struct house_min{
    bool is_known, is_ship, is_afloat, by_bot;
};

struct board{
    struct ship_list *afloat_ships, *sunken_ships;
    int size;
    struct house **square;
};

struct board_min{
    int size, afloat_ship_count;
    struct ship_tmp *afloat_ships;
    struct house_min **square;
};

#endif //SEABATTLE_BOARD_H
