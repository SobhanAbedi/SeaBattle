//
// Created by Sobhan on 1/23/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include "Board.h"

struct ship_tmp* init_ship_tmp(int len, int wid, int points)
{
    struct ship_tmp *ship = (struct ship_tmp*)malloc(sizeof(struct ship_tmp));
    ship->len = len;
    ship->wid = wid;
    ship->points = points;
    return ship;
}

struct config_ship_list* init_config_ship_list(struct ship_tmp *ship, int count)
{
    struct config_ship_list *list = (struct config_ship_list*)malloc(sizeof(struct config_ship_list));
    list->next = NULL;
    list->ship = ship;
    list->count = count;
    return list;
}

struct ship_list* init_ship_list(struct ship_tmp *ship, struct location *ploc)
{
    struct ship_list *list = (struct ship_list*)malloc(sizeof(struct ship_list));
    list->next = NULL;
    list->ship = ship;
    list->loc = *ploc;
    if(list->ship == NULL){
        list->is_afloat = false;
        list->health = NULL;
        return list;
    }
    list->is_afloat = true;
    list->health = (bool**)malloc(ship->wid * sizeof(bool*));
    for(int i = 0; i < ship->wid; i++){
        list->health[i] = (bool*)malloc(ship->len * sizeof(bool));
        for(int j = 0; j < ship->len; j++)
            list->health[i][j] = true;
    }
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

struct location_ext* get_location_ext(struct location *loc)
{
    struct location_ext* loc_ext = (struct location_ext*)malloc(sizeof(struct location_ext));
    loc_ext->x = loc->x;
    loc_ext->y = loc->y;
    int dx_list[] = {1, 0, -1, 0}, dy_list[] = {0, -1, 0, 1};
    loc_ext->dx_len = dx_list[loc->dir];
    loc_ext->dy_len = dy_list[loc->dir];
    loc_ext->dx_wid = dx_list[(loc->dir + 1) % 4];
    loc_ext->dy_wid = dy_list[(loc->dir + 1) % 4] ;
    return loc_ext;
}

bool can_place_ship(struct house** square, int board_size, struct ship_tmp *ship, struct location_ext *loc)
{
    int x, y;
    for(int i = -1; i < ship->wid + 1; i++)
        for(int j = -1; j < ship->len + 1; j++) {
            x = loc->x + i * loc->dx_wid + j * loc->dx_len;
            y = loc->y + i * loc->dy_wid + j * loc->dy_len;
            if(x < -1 || y < -1 || x > board_size || y > board_size) {
                printf("Out of Index\n");
                return 0;
            }
            if(x > -1 && y > -1 && x < board_size && y < board_size && square[y][x].is_ship) {
                printf("Ship OverLap\n");
                return 0;
            }
        }
    return 1;
}

bool place_ship(struct house** square, int board_size, struct ship_list *ship)
{
    struct location_ext *loc = get_location_ext(&(ship->loc));
    //printf("%d %d %d %d", loc->dx_len, loc->dy_len, loc->dx_wid, loc->dy_wid);
    if(!can_place_ship(square, board_size, ship->ship, loc))
        return 0;
    int x, y;
    for(int i = 0; i < ship->ship->wid; i++)
        for(int j = 0; j < ship->ship->len; j++) {
            x = loc->x + i * loc->dx_wid + j * loc->dx_len;
            y = loc->y + i * loc->dy_wid + j * loc->dy_len;
            square[y][x].is_ship = 1;
            square[y][x].is_visible = 0;
            square[y][x].health = &(ship->health[i][j]);
        }
    free(loc);
    return 1;
}

struct board* init_board(struct config *conf)
{
    //struct config *conf = get_conf();
    //Start get location
    int x[] = {0, 2, 2, 4, 4, 4, 6, 6, 6, 6};
    int y[] = {0, 0, 5, 0, 3, 6, 0, 2, 4, 6};
    //dir: 0->0deg, 1->90deg, 2->180deg, 3->270deg
    int dir[10] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    struct location loc = {-1, -1, -1};
    //End get location
    struct board* brd = (struct board*)malloc(sizeof(struct board));
    brd->size = conf->board_size;
    brd->square = (struct house**)malloc(brd->size * sizeof(struct house*));
    for(int i = 0; i < brd->size; i++) {
        brd->square[i] = (struct house*)malloc(brd->size * sizeof(struct house));
        for (int j = 0; j < brd->size; j++) {
            brd->square[i][j].health = NULL;
            brd->square[i][j].is_ship = 0;
            brd->square[i][j].is_visible = 0;
        }
    }
    brd->sunken_ships = init_ship_list(NULL, &loc);
    brd->afloat_ships = init_ship_list(NULL, &loc);
    struct ship_list *cur_ship = brd->afloat_ships;
    struct config_ship_list *cur_list = conf->ship_list->next;
    int k = 0;
    while(cur_list != NULL) {
        for(int i = 0; i < cur_list->count; i++){
            loc.x = x[k];
            loc.y = y[k];
            loc.dir = dir[k];
            k++;
            cur_ship->next = init_ship_list(cur_list->ship, &loc);
            cur_ship = cur_ship->next;
            if(!place_ship(brd->square, brd->size, cur_ship)){
                printf("Could not Initiate Board\n");
                destroy_board(brd);
                brd = NULL;
                return NULL;
            }
        }
        cur_list = cur_list->next;
    }
    return brd;
}

bool destroy_ship_list(struct ship_list *list)
{
    while(list->next != NULL)
        if(!destroy_ship_list(list->next))
            return false;
    list->next = NULL;
    //free(list->ship); dont free ship_tmp here. free ship_tmp in destroy_conf.
    for(int i = 0; i < list->ship->wid; i++){
        free(list->health[i]);
        list->health[i] = NULL;
    }
    free(list->health);
    list->health = NULL;
    free(list);
    return true;
}

bool destroy_board(struct board *brd)
{
    for(int i = 0; i < brd->size; i++){
        free(brd->square[i]);
        brd->square[i] = NULL;
    }
    free(brd->square);
    brd->square = NULL;
    if(destroy_ship_list(brd->afloat_ships) && destroy_ship_list(brd->sunken_ships)) {
        brd->afloat_ships = brd->sunken_ships = NULL;
        free(brd);
        return 1;
    }
}

