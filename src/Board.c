//
// Created by Sobhan on 1/23/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Board.h"
#include "Config.h"
#include "Android.h"

struct ship_tmp new_ship_tmp(int len, int wid, int points, struct asset *ast)
{
    struct ship_tmp ship;
    ship.len = len;
    ship.wid = wid;
    ship.points = points;
    ship.asset = ast;
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

int fill_board_random(struct board *brd, struct config_ship_list *conf_ship_list)
{
    struct location null_loc = {-1, -1, -1};
    brd->afloat_ships = new_ship_list_ent(NULL, &null_loc);
    struct config_ship_list *cur_list = conf_ship_list->next;
    struct ship_list *cur_ship = brd->afloat_ships;
    while(cur_list != NULL) {
        for (int i = 0; i < cur_list->count; i++) {
            cur_ship->next = new_ship_list_ent(cur_list->ship, &null_loc);
            cur_ship = cur_ship->next;
        }
        cur_list = cur_list->next;
    }
    struct location *ships_loc;
    int count = fill_board(brd, &ships_loc, 100);

    destroy_ship_list(brd->afloat_ships);
    brd->afloat_ships =NULL;
    brd->afloat_ships = new_ship_list_ent(NULL, &null_loc);
    cur_list = conf_ship_list->next;
    cur_ship = brd->afloat_ships;
    int k = 0;
    while(cur_list != NULL && k < count) {
        for (int i = 0; i < cur_list->count; i++) {
            cur_ship->next = new_ship_list_ent(cur_list->ship, &(ships_loc[k]));
            cur_ship = cur_ship->next;
            if (!place_ship(brd->square, brd->size, cur_ship)) {
                free(ships_loc);
                return -1;
            }
            k++;
        }
        cur_list = cur_list->next;
    }
    free(ships_loc);
    return count;
}

struct board* init_board()
{
    struct config *conf = get_conf();
    //Start get location
    int x[] = {0, 2, 2, 4, 4, 4, 6, 6, 6, 6};
    int y[] = {0, 0, 4, 0, 3, 6, 0, 2, 4, 6};
    //dir: 0->0deg, 1->90deg, 2->180deg, 3->270deg
    int dir[10] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    struct location null_loc = {-1, -1, -1};
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
    int k = fill_board_random(brd, conf->ship_list);
    if (k == -1) {
        printf("Could not Initiate Board\n");
        destroy_board(brd);
        brd = NULL;
        return NULL;
    }
    brd->sunken_ships = new_ship_list_ent(NULL, &null_loc);

    //struct ship_list *cur_ship = brd->afloat_ships;
    //struct config_ship_list *cur_list = conf->ship_list->next;
    /*
    int k = 0;
    while(cur_list != NULL) {
        for (int i = 0; i < cur_list->count; i++) {
            loc.x = x[k];
            loc.y = y[k];
            loc.dir = dir[k];
            k++;
            cur_ship->next = new_ship_list_ent(cur_list->ship, &loc);
            cur_ship = cur_ship->next;
            if (!place_ship(brd->square, brd->size, cur_ship)) {
                printf("Could not Initiate Board\n");
                destroy_board(brd);
                brd = NULL;
                return NULL;
            }
        }
        cur_list = cur_list->next;
    }
    */
    destroy_config_ship_list(conf->ship_list);
    return brd;
}

bool write_ship_list2file(struct ship_list *list, FILE *fout)
{
    struct ship_tmp *ship_temps = get_ship_temps();
    struct ship_list *cur = list->next;
    int count = 0;
    while (cur != NULL) {
        count++;
        cur = cur->next;
    }
    if(fwrite(&count, sizeof(int), 1, fout) == 0)
        return false;
    cur = list->next;
    while(cur != NULL) {
        int index = cur->ship - ship_temps;
        if(fwrite(&index, sizeof(int), 1, fout) == 0)
            return false;
        if(fwrite(&(cur->loc), sizeof(struct location), 1, fout) == 0)
            return false;
        if(fwrite(&(cur->is_afloat), sizeof(bool), 1, fout) == 0)
            return false;
        if(cur->is_afloat) {
            for (int i = 0; i < cur->ship->wid; i++)
                if (fwrite(cur->health[i], sizeof(bool), cur->ship->len, fout) < cur->ship->len)
                    return false;
        }
        cur = cur->next;
    }
    return true;
}

struct ship_list* read_ship_list_from_file(struct board *brd, int *points, FILE *fin)
{
    int count;
    if(fread(&count, sizeof(int), 1, fin) == 0)
        return NULL;
    struct ship_tmp *ship_temps = get_ship_temps();
    struct location loc = {-1, -1, -1};
    struct ship_list *list_beg = new_ship_list_ent(NULL, &loc), *cur = list_beg;
    int index;
    for(int i = 0; i < count; i++){
        if(fread(&index, sizeof(int), 1, fin) == 0 || fread(&loc, sizeof(struct location), 1, fin) == 0){
            destroy_ship_list(list_beg);
            list_beg = NULL;
            return list_beg;
        }
        cur->next = new_ship_list_ent(&(ship_temps[index]), &loc);
        cur = cur->next;
        if(fread(&(cur->is_afloat), sizeof(bool), 1, fin) == 0){
            destroy_ship_list(list_beg);
            list_beg = NULL;
            return list_beg;
        }
        if(cur->is_afloat){
            for (int j = 0; j < cur->ship->wid; j++) {
                if (fread(cur->health[j], sizeof(bool), cur->ship->len, fin) < cur->ship->len) {
                    destroy_ship_list(list_beg);
                    list_beg = NULL;
                    return list_beg;
                }
                for(int k = 0; k < cur->ship->len; k++)
                    *points += (!cur->health[j][k]);
            }
        } else {
            for(int j = 0; j < cur->ship->wid; j++)
                for(int k = 0; k < cur->ship->len; k++)
                    cur->health[j][k] = false;
            *points += ((cur->ship->wid) * (cur->ship->len)) + cur->ship->points;
        }
        place_ship(brd->square, brd->size, cur);
    }
    return list_beg;
}

bool write_house2file(struct board *brd, FILE *fout)
{
    for(int i = 0 ; i < brd->size; i++)
        for(int j = 0; j < brd->size; j++)
            if(fwrite(&(brd->square[i][j].is_visible), sizeof(bool), 1, fout) == 0)
                return false;
    return true;
}

struct house** init_house_from_file(int size, FILE *fin)
{
    struct house **sq = (struct house**)malloc(size * sizeof(struct house*));
    for(int i = 0; i < size; i++){
        sq[i] = (struct house*)malloc(size * sizeof(struct house));
        for(int j = 0; j < size; j++){
            if(fread(&(sq[i][j].is_visible), sizeof(bool), 1, fin) == 0){
                destroy_house(sq, size);
                sq = NULL;
                return sq;
            }
            sq[i][j].is_ship = false;
            sq[i][j].is_afloat = NULL;
            sq[i][j].health = NULL;
        }
    }
    return sq;
}

bool write_board2file(struct board *brd, FILE *fout)
{
    if(fwrite(&(brd->size), sizeof(int), 1, fout) == 1 && write_house2file(brd, fout)
        && write_ship_list2file(brd->afloat_ships, fout) && write_ship_list2file(brd->sunken_ships, fout))
        return true;
    return false;
}

struct board* read_board_from_file(int *points, FILE *fin)
{
    struct board* brd = (struct board*)malloc(sizeof(struct board));
    if(fread(&(brd->size), sizeof(int), 1, fin) == 0) {
        free(brd);
        return NULL;
    }
    brd->square = init_house_from_file(brd->size, fin);
    if(brd->square == NULL) {
        destroy_board(brd);
        brd = NULL;
        return brd;
    }
    brd->afloat_ships = read_ship_list_from_file(brd, points, fin);
    if(brd->afloat_ships == NULL){
        destroy_board(brd);
        brd = NULL;
        return brd;
    }
    brd->sunken_ships = read_ship_list_from_file(brd, points, fin);
    if(brd->sunken_ships == NULL){
        destroy_board(brd);
        brd = NULL;
        return brd;
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
                    printf(" ");
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

struct board_min* get_board_for_bot(struct board *brd)
{
    struct board_min *brd_min = (struct board_min*)malloc(sizeof(struct board_min));
    brd_min->size = brd->size;
    brd_min->square = (struct house_min**)malloc(brd_min->size * sizeof(struct house_min*));
    for(int i = 0; i < brd_min->size; i++){
        brd_min->square[i] = (struct house_min*)malloc(brd_min->size * sizeof(struct house_min));
        for(int j = 0; j < brd_min->size; j++){
            brd_min->square[i][j].is_known = brd->square[i][j].is_visible;
            brd_min->square[i][j].is_ship = brd_min->square[i][j].by_bot = brd_min->square[i][j].is_afloat = false;
            if(brd_min->square[i][j].is_known){
                brd_min->square[i][j].is_ship = brd->square[i][j].is_ship;
                if(brd->square[i][j].is_ship)
                    brd_min->square[i][j].is_afloat = *(brd->square[i][j].is_afloat);
            }
        }
    }
    int count = 0;
    struct ship_list *cur = brd->afloat_ships->next;
    while(cur != NULL){
        cur = cur->next;
        count++;
    }
    brd_min->afloat_ship_count = count;
    cur = brd->afloat_ships->next;
    brd_min->afloat_ships = (struct ship_tmp*)malloc(count * sizeof(struct ship_tmp));
    for(int i = 0; i < count && cur != NULL; i++){
        brd_min->afloat_ships[i].wid = cur->ship->wid;
        brd_min->afloat_ships[i].len = cur->ship->len;
        brd_min->afloat_ships[i].points = cur->ship->points;
        brd_min->afloat_ships[i].asset = NULL;
        cur = cur->next;
    }
    return brd_min;
}

void make_board_visible(struct board *brd)
{
    for(int i = 0; i < brd->size; i++)
        for(int j = 0; j < brd->size; j++)
            brd->square[i][j].is_visible = true;
}

void destroy_ship_list(struct ship_list *list)
{
    if(list->next == NULL)
        return;
    destroy_ship_list(list->next);
    list->next = NULL;
    //free(list->ship); dont free ship_tmp here. free ship_tmp in destroy_conf.
    if(list->ship != NULL && list->health != NULL) {
        for (int i = 0; i < list->ship->wid; i++) {
            free(list->health[i]);
            list->health[i] = NULL;
        }
        free(list->health);
        list->health = NULL;
    }
    free(list);
}

void destroy_house(struct house** square, int size)
{
    if(square == NULL)
        return;
    for(int i = 0; i < size; i++){
        free(square[i]);
        square[i] = NULL;
    }
    free(square);
}

void destroy_board(struct board *brd)
{
    destroy_house(brd->square, brd->size);
    brd->square = NULL;
    destroy_ship_list(brd->afloat_ships);
    destroy_ship_list(brd->sunken_ships);
    brd->afloat_ships = NULL;
    brd->sunken_ships = NULL;
    free(brd);
}

void destroy_board_min(struct board_min *brd, bool is_final)
{
    if(is_final) {
        free(brd->afloat_ships);
    }
    brd->afloat_ships = NULL;
    for(int i = 0; i < brd->size; i++){
        free(brd->square[i]);
        brd->square[i] = NULL;
    }
    free(brd->square);
    brd->square = NULL;
    free(brd);
}