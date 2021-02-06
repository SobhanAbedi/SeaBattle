//
// Created by Sobhan on 1/27/2021.
//

#ifndef SEABATTLE_ANDROID_H
#define SEABATTLE_ANDROID_H

#include <stdbool.h>
#include "Board.h"

//struct declarations
struct android;

//function declarations
struct android* init_bot();
void disp_android_fast(struct android *bot, bool debug);
bool write_bot2file(struct android *bot, FILE *fout);
struct android* read_bot_from_file(int *points, FILE *fin);
void board_cpy(struct board_min *dst, struct board_min *src);
int bot_can_place_ship(struct board_min *brd, struct location *loc, int ship_num, int****loc_pos , bool place);
int**** get_location_possibility(struct board_min *brd);
int**** copy_location_possibility(struct board_min *brd, int ****src, int from);
void merge_location_possibility(struct board_min *brd, int ****dst, int ****src, int from);
void reset_location_possibility(struct board_min *brd, int ****loc_pos, int ship_num);
bool get_next_location_sequential(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num);
bool get_next_location_random(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num);
bool get_next_location_E_priority(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num, struct location *assist_loc);
bool board_has_Es(struct board_min *brd);
bool get_next_location(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num, bool random, struct location *assist_loc);
bool fill_board_itr(struct board_min *main_brd, int ****loc_pos, int ship_num, int **res_board, bool random, double max_no_ans, int *found_count, double max_found, int max_uses);
bool can_fill_board(struct board *brd, double max_no_ans);
struct location get_hit_loc(struct board *brd, int max_no_ans, double max_found);
void destroy_android(struct android *bot);
void destroy_location_possibility(struct board_min *brd, int ****loc_pos);
//struct definitions
struct android
{
    struct board *brd;
    int points;
};

#endif //SEABATTLE_ANDROID_H
