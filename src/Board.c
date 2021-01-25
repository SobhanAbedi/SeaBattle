//
// Created by Sobhan on 1/23/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include "Board.h"
#include "Config.h"

struct ship_tmp* new_ship_tmp(int len, int wid, int points)
{
    struct ship_tmp *ship = (struct ship_tmp*)malloc(sizeof(struct ship_tmp));
    ship->len = len;
    ship->wid = wid;
    ship->points = points;
    return ship;
}

struct ship_list* new_ship_list_ent(struct ship_tmp *ship, struct location *ploc)
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
            square[y][x].is_afloat = &(ship->is_afloat);
        }
    free(loc);
    return 1;
}

void make_visible_around_sunken_ship(struct board *brd, struct ship_list *ent)
{
    int x, y;
    struct location_ext *loc = get_location_ext(&(ent->loc));
    for(int i = -1; i <= ent->ship->wid; i++){
        x = loc->x + i * loc->dx_wid + (-1) * loc->dx_len;
        y = loc->y + i * loc->dy_wid + (-1) * loc->dy_len;
        if(x >= 0 && y >= 0 && x < brd->size && y < brd->size)
            brd->square[y][x].is_visible = true;
        x = loc->x + i * loc->dx_wid + (ent->ship->len) * loc->dx_len;
        y = loc->y + i * loc->dy_wid + (ent->ship->len) * loc->dy_len;
        if(x >= 0 && y >= 0 && x < brd->size && y < brd->size)
            brd->square[y][x].is_visible = true;
    }
    for(int i = 0; i < ent->ship->len; i++){
        x = loc->x + (-1) * loc->dx_wid + i * loc->dx_len;
        y = loc->y + (-1) * loc->dy_wid + i * loc->dy_len;
        if(x >= 0 && y >= 0 && x < brd->size && y < brd->size)
            brd->square[y][x].is_visible = true;
        x = loc->x + (ent->ship->wid) * loc->dx_wid + i * loc->dx_len;
        y = loc->y + (ent->ship->wid) * loc->dy_wid + i * loc->dy_len;
        if(x >= 0 && y >= 0 && x < brd->size && y < brd->size)
            brd->square[y][x].is_visible = true;
    }
}

int check_afloat_ships(struct board *brd)
{
    struct ship_list *cur_afloat = brd->afloat_ships, *cur_sunken = brd->sunken_ships ,*to_rem;
    bool is_sunken;
    while(cur_afloat->next != NULL){
        is_sunken = true;
        for(int i = 0; i < cur_afloat->next->ship->wid; i++)
            for(int j = 0; j < cur_afloat->next->ship->len; j++)
                if(cur_afloat->next->health[i][j]){
                    is_sunken = false;
                    break;
                }
        if(is_sunken){
            while(cur_sunken->next != NULL)
                cur_sunken = cur_sunken->next;
            to_rem = cur_afloat->next;
            cur_afloat->next = cur_afloat->next->next;
            to_rem->is_afloat = false;
            to_rem->next = NULL;
            cur_sunken->next = to_rem;
            make_visible_around_sunken_ship(brd, to_rem);
            return to_rem->ship->points;
        }
        cur_afloat = cur_afloat->next;
    }
    return 0;
}

struct ship_list* copy_ship_list(struct ship_list *list1)
{
    struct location loc = {-1, -1, -1};
    struct ship_list *list2 = new_ship_list_ent(NULL, &loc);
    list1 = list1->next;
    while(list1 != NULL){
        list2->next = new_ship_list_ent(list1->ship, &(list1->loc));
        list1 = list1->next;
        list2 = list2->next;
    }
    return list2;
}

bool rem_ship_list_ent(struct ship_list *pre)
{
    if(pre->next == NULL)
        return 0;
    struct ship_list *to_rem = pre->next;
    pre->next = pre->next->next;
    for(int i = 0; i < to_rem->ship->wid; i++){
        free(to_rem->health[i]);
        to_rem->health[i] = NULL;
    }
    free(to_rem->health);
    to_rem->health = NULL;
    free(to_rem);
    return true;
}

struct board* init_board()
{
    struct config *conf = get_conf();
    //Start get location
    int x[] = {0, 2, 2, 4, 4, 4, 6, 6, 6, 6};
    int y[] = {0, 0, 4, 0, 3, 6, 0, 2, 4, 6};
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
    brd->sunken_ships = new_ship_list_ent(NULL, &loc);
    brd->afloat_ships = new_ship_list_ent(NULL, &loc);
    struct ship_list *cur_ship = brd->afloat_ships;
    struct config_ship_list *cur_list = conf->ship_list->next;
    int k = 0;
    while(cur_list != NULL) {
        for(int i = 0; i < cur_list->count; i++){
            loc.x = x[k];
            loc.y = y[k];
            loc.dir = dir[k];
            k++;
            cur_ship->next = new_ship_list_ent(cur_list->ship, &loc);
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

void disp_board_fast(struct board *brd, bool debug)
{
    for(int i = 0; i < brd->size; i++){
        for(int j = 0; j < brd->size; j++){
            if(!debug) {
                if (brd->square[i][j].is_visible) {
                    if (!brd->square[i][j].is_ship)
                        printf("W");
                    else {
                        if (!*(brd->square[i][j].is_afloat))
                            printf("C");
                        else
                            printf("E");
                    }
                } else
                    printf("H");
                printf(" ");
            }else {
                if (brd->square[i][j].is_ship)
                    printf("S");
                else
                    printf("W");
                if (brd->square[i][j].is_visible)
                    printf(".");
                else
                    printf(" ");
            }
        }
        printf("\n");
    }
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

