//
// Created by Sobhan on 1/27/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include "Android.h"

struct android* init_bot()
{
    struct android* bot = (struct android*)malloc(sizeof(struct android));
    bot->points = 0;
    bot->brd = init_board();
    return bot;
}

void disp_android_fast(struct android *bot, bool debug)
{
    printf("Bot Has %d  Points in Current Game\n", bot->points);
    disp_board_fast(bot->brd, debug);
}

bool write_bot2file(struct android *bot, FILE *fout)
{
    return write_board2file(bot->brd, fout);
}

struct android* read_bot_from_file(int *points, FILE *fin)
{
    struct android *bot = (struct android*)malloc(sizeof(struct android));
    bot->brd = read_board_from_file(points, fin);
    if(bot->brd == NULL){
        destroy_android(bot);
        bot = NULL;
        return bot;
    }
    bot->points = 0;
    return bot;
}

void board_cpy(struct board_min *dst, struct board_min *src)
{
    if(dst->size == 0 || dst->afloat_ship_count == 0 || dst->afloat_ships == NULL || dst->square == NULL){
        dst->size = src->size;
        dst->afloat_ship_count = src->afloat_ship_count;
        dst->afloat_ships = src->afloat_ships;
        dst->square = (struct house_min**)malloc(dst->size * sizeof(struct house_min*));
        for(int i = 0; i < dst->size; i++)
            dst->square[i] = (struct house_min*)malloc(dst->size * sizeof(struct house_min));
    }
    for(int i = 0; i < dst->size; i++) {
        for (int j = 0; j < dst->size; j++)
            dst->square[i][j] = src->square[i][j];
    }
}

int bot_can_place_ship(struct board_min *brd, struct location *loc, int ship_num, int****loc_pos , bool place) //return 1:yes, 0:relative no, -1:absolute no
{
    bool can_place = true;
    struct location_ext *loc_ext = get_location_ext(loc);
    for(int i = -1; i <= brd->afloat_ships[ship_num].wid; i++){
        for(int j = -1; j <= brd->afloat_ships[ship_num].len; j++){
            int x = loc_ext->x + i * (loc_ext->dx_wid) + j * (loc_ext->dx_len);
            int y = loc_ext->y + i * (loc_ext->dy_wid) + j * (loc_ext->dy_len);

            if(i == -1 || j == -1 || i == brd->afloat_ships[ship_num].wid || j == brd->afloat_ships[ship_num].len){
                if(x >= 0 && x < brd->size && y >= 0 && y < brd->size) {
                    if(place){
                        if(!brd->square[y][x].is_known)
                            brd->square[y][x].by_bot = true;
                        brd->square[y][x].is_known = true;
                        brd->square[y][x].is_ship = false;
                        for(int n = ship_num + 1; n < brd->afloat_ship_count; n++)
                            for(int k = 0; k < 4; k++)
                                loc_pos[n][y][x][k] = 0;
                    } else if (brd->square[y][x].is_known && brd->square[y][x].is_ship) {
                        if(!brd->square[y][x].by_bot) {
                            free(loc_ext);
                            return -1;
                        }
                        can_place = false;
                    }
                }
            } else{
                if(x < 0 || x >= brd->size || y < 0 || y >= brd->size) {
                    return -1;
                }
                if(place){
                    if(!brd->square[y][x].is_known)
                        brd->square[y][x].by_bot = true;
                    brd->square[y][x].is_known = true;
                    brd->square[y][x].is_ship = true;
                    brd->square[y][x].is_afloat = false;
                    for(int n = ship_num + 1; n < brd->afloat_ship_count; n++)
                        for(int k = 0; k < 4; k++)
                            loc_pos[n][y][x][k] = 0;
                } else if(brd->square[y][x].is_known && !(brd->square[y][x].is_ship && brd->square[y][x].is_afloat)){
                    if(!brd->square[y][x].by_bot) {
                        free(loc_ext);
                        return -1;
                    }
                    can_place = false;
                }
            }
        }
    }
    free(loc_ext);
    if(can_place)
        return 1;
    return 0;
}

int**** get_location_possibility(struct board_min *brd)
{
    int ****loc_pos = (int****)malloc(brd->afloat_ship_count * sizeof(int***));
    for(int n = 0; n < brd->afloat_ship_count; n++){
        loc_pos[n] = (int***)malloc(brd->size * sizeof(int**));
        for(int i = 0; i < brd->size; i++){
            loc_pos[n][i] = (int**)malloc(brd->size * sizeof(int*));
            for(int j = 0; j < brd->size; j++){
                loc_pos[n][i][j] = (int*)malloc(4 * sizeof(int));
                int pos;
                if(brd->square[i][j].is_known && !(brd->square[i][j].is_ship && brd->square[i][j].is_afloat))
                    pos = -1;
                else
                    pos = 1;
                for(int k = 0; k < 4; k++)
                    loc_pos[n][i][j][k] = pos;
            }
        }
    }
    return loc_pos;
}

int**** copy_location_possibility(struct board_min *brd, int ****src, int from)
{
    int ****dst = (int****)malloc(brd->afloat_ship_count * sizeof(int***));
    for(int n = from; n < brd->afloat_ship_count; n++) {
        dst[n] = (int ***) malloc(brd->size * sizeof(int **));
        for(int i = 0; i < brd->size; i++) {
            dst[n][i] = (int **) malloc(brd->size * sizeof(int *));
            for(int j = 0; j < brd->size; j++) {
                dst[n][i][j] = (int *) malloc(4 * sizeof(int));
                for(int k = 0; k < 4; k++)
                    dst[n][i][j][k] = src[n][i][j][k];
            }
        }
    }
    return dst;
}

void merge_location_possibility(struct board_min *brd, int ****dst, int ****src, int from)
{
    for(int n = from; n < brd->afloat_ship_count; n++) {
        for (int i = 0; i < brd->size; i++) {
            for (int j = 0; j < brd->size; j++) {
                for (int k = 0; k < 4; k++) {
                    if (src[n][i][j][k] == -1)
                        dst[n][i][j][k] = -1;
                }
                free(src[n][i][j]);
                src[n][i][j] = NULL;
            }
            free(src[n][i]);
            src[n][i] = NULL;
        }
        free(src[n]);
        src[n] = NULL;
    }
    free(src);
}

void reset_location_possibility(struct board_min *brd, int ****loc_pos, int ship_num)
{
    for(int i = 0; i < brd->size; i++)
        for(int j = 0; j < brd->size; j++)
            for(int k = 0; k < 4; k++)
                if(loc_pos[ship_num][i][j][k] == 0)
                    loc_pos[ship_num][i][j][k] = 1;
    /*
    for(int n = ship_num; n < brd->afloat_ship_count; n++) {
        for (int i = 0; i < brd->size; i++) {
            for (int j = 0; j < brd->size; j++) {
                int pos;
                if (brd->square[i][j].is_known && !(brd->square[i][j].is_ship && brd->square[i][j].is_afloat)) {
                    if (brd->square[i][j].by_bot)
                        pos = 0;
                    else
                        pos = -1;
                } else {
                    pos = 1;
                }
                for (int k = 0; k < 4; k++) {
                    if (loc_pos[n][i][j][k] != -1) {
                        loc_pos[n][i][j][k] = pos;
                    }
                }
            }
        }
    }
    */
}

bool get_next_location_sequential(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num)
{
    int i = loc->y;
    int j = loc->x;
    int k = loc->dir + 1;
    for (; i < brd->size; i++) {
        for (; j < brd->size; j++) {
            for (; k < 4; k++) {
                if (loc_pos[ship_num][i][j][k] == 1) {
                    loc->y = i;
                    loc->x = j;
                    loc->dir = k;
                    return true;
                }
            }
            k = 0;
        }
        j = 0;
    }
    return false;
}

bool get_next_location_random(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num)
{
    int available_locs = 0;
    for(int i = 0; i < brd->size; i++){
        for(int j = 0; j < brd->size; j++){
            for(int k = 0; k < 4; k++)
                if(loc_pos[ship_num][i][j][k] == 1)
                    available_locs++;
        }
    }
    if(available_locs == 0)
        return false;
    int desired_loc = (int)(available_locs * (((double)rand())/RAND_MAX));
    if(desired_loc == available_locs)
        desired_loc--;
    for(int i = 0; i < brd->size && desired_loc >= 0; i++){
        for(int j = 0; j < brd->size && desired_loc >= 0; j++){
            for(int k = 0; k < 4 && desired_loc >= 0; k++){
                if(loc_pos[ship_num][i][j][k] == 1){
                    if(desired_loc == 0){
                        loc->y = i;
                        loc->x = j;
                        loc->dir = k;
                        return true;
                    }
                    desired_loc--;
                }
            }
        }
    }
    return false;
}

bool get_next_location_E_priority(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num, struct location *assist_loc)
{
    //printf("got into E_priority\n");
    struct location *cur_loc = (struct location*)malloc(sizeof(struct location));
    struct location_ext *cur_loc_ext = (struct location_ext*)malloc(sizeof(struct location_ext));
    int m = assist_loc[0].y, n = assist_loc[0].x, i = assist_loc[1].y, j = assist_loc[1].x, k = assist_loc[1].dir + 1;
    for(; m < brd->size; m++){
        for(; n < brd->size; n++){
            if(!(brd->square[m][n].is_known && brd->square[m][n].is_ship && brd->square[m][n].is_afloat))
                continue;
            cur_loc->y = m;
            cur_loc->x = n;
            for(; i < brd->afloat_ships[ship_num].wid; i++){
                for(; j < brd->afloat_ships[ship_num].len; j++){
                    for(; k < 4; k++){
                        cur_loc->dir = k;
                        cur_loc_ext = get_location_ext(cur_loc);
                        int x = cur_loc_ext->x - i * (cur_loc_ext->dx_wid) - j * (cur_loc_ext->dx_len);
                        int y = cur_loc_ext->y - i * (cur_loc_ext->dy_wid) - j * (cur_loc_ext->dy_len);
                        if(x < 0 || y < 0 || x >= brd->size || y >= brd->size)
                            continue;
                        if(loc_pos[ship_num][y][x][k] == 1){
                            loc->x = x;
                            loc->y = y;
                            loc->dir = k;
                            assist_loc[0].y = m;
                            assist_loc[0].x = n;
                            assist_loc[1].y = i;
                            assist_loc[1].x = j;
                            assist_loc[1].dir = k;
                            free(cur_loc);
                            free(cur_loc_ext);
                            //for(int i = 0; i < ship_num; i++)
                            //    printf("  ");
                            //printf("%dx%d@%d hit\n", loc->y, loc->x, loc->dir);
                            return true;
                        }
                    }
                    k = 0;
                }
                j = 0;
            }
            i = 0;
        }
        n = 0;
    }
    assist_loc[0].y = m;
    assist_loc[0].x = n;
    assist_loc[1].y = i;
    assist_loc[1].x = j;
    assist_loc[1].dir = k;
    free(cur_loc);
    free(cur_loc_ext);
    return false;
}

bool board_has_Es(struct board_min *brd)
{
    for(int i = 0; i < brd->size; i++)
        for(int j = 0; j < brd->size; j++)
            if(brd->square[i][j].is_known && brd->square[i][j].is_ship && brd->square[i][j].is_afloat)
                return true;
    return false;
}

bool get_next_location(struct board_min *brd, struct location *loc, int****loc_pos, int ship_num, bool random, struct location *assist_loc)
{
    if(!random) {
        return get_next_location_sequential(brd, loc, loc_pos, ship_num);
    }
    bool res = false;
    struct location *temp_loc = (struct location*)malloc(sizeof(struct location));
    temp_loc->x = loc->x;
    temp_loc->y = loc->y;
    temp_loc->dir = loc->dir;
    if(brd->afloat_ships[ship_num].len > 1 && board_has_Es(brd))
        res = get_next_location_E_priority(brd, temp_loc, loc_pos, ship_num, assist_loc);
    if(!res){
        res = get_next_location_random(brd, temp_loc, loc_pos, ship_num);
        if(!res){
            free(temp_loc);
            return get_next_location_sequential(brd, loc, loc_pos, ship_num);
        } else{
            loc->x = temp_loc->x;
            loc->y = temp_loc->y;
            loc->dir = temp_loc->dir;
        }
    } else{
        loc->x = temp_loc->x;
        loc->y = temp_loc->y;
        loc->dir = temp_loc->dir;
    }
    free(temp_loc);
    return true;
}

bool fill_board_itr(struct board_min *main_brd, int ****loc_pos, int ship_num, int **res_board, bool random, double max_no_ans, int *found_count, double max_found, int max_uses)
{
    //for(int i = 0; i < ship_num; i++)
    //    printf("  ");
    //printf("%d %dx%d\n", ship_num, main_brd->afloat_ships[ship_num].wid, main_brd->afloat_ships[ship_num].len);
    if(ship_num == main_brd->afloat_ship_count) {
        for (int i = 0; i < main_brd->size; i++) {
            for (int j = 0; j < main_brd->size; j++)
                if (main_brd->square[i][j].is_known && main_brd->square[i][j].is_ship && main_brd->square[i][j].is_afloat)
                    return false;
        }
        for(int i = 0; i < main_brd->size; i++){
            for(int j = 0; j < main_brd->size; j++)
                if(main_brd->square[i][j].is_known && main_brd->square[i][j].is_ship && main_brd->square[i][j].by_bot)
                    res_board[i][j]++;
        }
        (*found_count)++;
        return true;
    }
    struct board_min *cur_brd = (struct board_min*)malloc(sizeof(struct board_min));
    cur_brd->afloat_ship_count = cur_brd->size = 0;
    board_cpy(cur_brd, main_brd);
    bool res = false;
    struct location *loc = (struct location*)malloc(sizeof(struct location));
    struct location *assist_loc = (struct location*)malloc(3 * sizeof(struct location));
    loc->x = loc->y = 0;
    loc->dir = -1;
    assist_loc[0].x = assist_loc[0].y = 0;
    assist_loc[0].dir = -1;
    assist_loc[1].x = assist_loc[1].y = 0;
    assist_loc[1].dir = -1;
    int uses = 0, no_ans_count = 0;
    while((uses < max_uses || ship_num == 0) && get_next_location(cur_brd, loc, loc_pos, ship_num, random, assist_loc)){
        int possible = bot_can_place_ship(cur_brd, loc, ship_num, loc_pos, 0);
        bool temp_res;
        if (possible == 1){
            //int ****temp_loc_pos = copy_location_possibility(main_brd, loc_pos, ship_num + 1);
            bot_can_place_ship(cur_brd, loc, ship_num, loc_pos, 1);
            temp_res = fill_board_itr(cur_brd, loc_pos, ship_num + 1, res_board, random, max_no_ans * 1.2, found_count, max_found, max_uses);
            //merge_location_possibility(main_brd, loc_pos, temp_loc_pos, ship_num + 1);
            if(temp_res)
                uses++;
            else {
                no_ans_count++;
                possible = 0;
            }
            res = res || temp_res;
            //for(int i = 0; i < ship_num; i++)
            //    printf("  ");
            //printf("%d\n", ship_num);
            board_cpy(cur_brd, main_brd);
            if(ship_num + 1 < main_brd->afloat_ship_count)
                reset_location_possibility(main_brd, loc_pos, ship_num + 1);

            if((*found_count) >= max_found || no_ans_count >= max_no_ans){
                //printf("%d found, %d no ans\n", *found_count, no_ans_count);
                destroy_board_min(cur_brd, false);
                free(loc);
                free(assist_loc);
                return res;
            }

        }

        loc_pos[ship_num][loc->y][loc->x][loc->dir] = possible;
        if(main_brd->afloat_ships[ship_num].len == 1)
            for(int kp = 0; kp < 4; kp++)
                loc_pos[ship_num][loc->y][loc->x][kp] = possible;
        if((*found_count) == 0 && ship_num != 0 && uses >= max_uses)
            printf("!!!Exit Due to MAX_USES!!!\n");
    }
    destroy_board_min(cur_brd, false);
    free(loc);
    free(assist_loc);
    return res;
}

bool can_fill_board(struct board *brd, double max_no_ans)
{
    struct board_min *brd_min = get_board_for_bot(brd);
    //printf("board_min made\n");
    int found_count = 0;
    int **res_board = (int**)malloc(brd_min->size * sizeof(int*));
    for(int i = 0; i < brd_min->size; i++){
        res_board[i] = (int*)malloc(brd_min->size * sizeof(int));
        for(int j = 0; j < brd_min->size; j++)
            res_board[i][j] = 0;
    }
    //printf("res_board made\n");
    int ****loc_pos = get_location_possibility(brd_min);
    //printf("got location_possibility\n");
    bool ans = fill_board_itr(brd_min, loc_pos,0, res_board, false, max_no_ans, &found_count, 1, 2);

    for(int i = 0 ; i < brd_min->size; i++){
        for(int j = 0; j < brd_min->size; j++)
            printf("%-2d", res_board[i][j]);
        printf("\n");
    }

    for(int i = 0; i < brd_min->size; i++){
        free(res_board[i]);
        res_board[i] = NULL;
    }
    free(res_board);
    res_board = NULL;
    destroy_location_possibility(brd_min, loc_pos);
    destroy_board_min(brd_min, true);
    return ans;
}

struct location get_hit_loc(struct board *brd, int max_no_ans, double max_found)
{

    //printf("Hi\n");
    struct board_min *brd_min = get_board_for_bot(brd);
    //printf("Trying to place %d ships\n", brd_min->afloat_ship_count);
    //printf("board_min made\n");
    int found_count = 0;
    int **res_board = (int**)malloc(brd_min->size * sizeof(int*));
    for(int i = 0; i < brd_min->size; i++){
        res_board[i] = (int*)malloc(brd_min->size * sizeof(int));
        for(int j = 0; j < brd_min->size; j++)
            res_board[i][j] = 0;
    }
    //printf("res_board made\n");
    int ****loc_pos = get_location_possibility(brd_min);
    //printf("got location_possibility\n");
    bool ans = fill_board_itr(brd_min, loc_pos, 0, res_board, true, (double)max_no_ans, &found_count, max_found, 1);
    //if(!ans)
    //    ans = fill_board_itr(brd_min, loc_pos, 0, res_board, false, &no_ans_count, max_no_ans, &found_count, 10000, 3);
    //printf("%d Answers Found\n", found_count);
    struct location max_loc, min_loc;
    max_loc.x = max_loc.y = max_loc.dir = -1;
    min_loc.x = min_loc.y = min_loc.dir = -1;
    if(ans) {
        max_loc.x = max_loc.y = 0;
        max_loc.dir = res_board[0][0];
        min_loc.x = min_loc.y = 0;
        min_loc.dir = res_board[0][0];
        for (int i = 0; i < brd_min->size; i++) {
            for (int j = 0; j < brd_min->size; j++) {
                if(res_board[i][j] > max_loc.dir){
                    max_loc.dir = res_board[i][j];
                    max_loc.y = i;
                    max_loc.x = j;
                }
                if(res_board[i][j] < min_loc.dir){
                    min_loc.dir = res_board[i][j];
                    min_loc.y = i;
                    min_loc.x = j;
                }
            }
        }
    }
    printf("Max Value: %d\n", max_loc.dir);
    int available_locs = 0;
    for(int i = 0; i < brd_min->size; i++){
        for(int j = 0; j < brd_min->size; j++) {
            if((((double)(res_board[i][j] - min_loc.dir))/(max_loc.dir - min_loc.dir)) >= 0.8)
                available_locs++;
            printf("%-2d", (int) (10 * (((double)(res_board[i][j]) - min_loc.dir) / (max_loc.dir - min_loc.dir))));
        }
        printf("\n");
    }
    int desired_loc = (int)(available_locs * (((double)rand())/RAND_MAX));
    if(desired_loc == available_locs)
        desired_loc--;
    for(int i = 0; i < brd_min->size && desired_loc >= 0; i++){
        for(int j = 0; j < brd_min->size && desired_loc >= 0; j++){
            if((((double)(res_board[i][j] - min_loc.dir))/(max_loc.dir - min_loc.dir)) >= 0.8){
                if(desired_loc == 0){
                    max_loc.y = i;
                    max_loc.x = j;
                }
                desired_loc--;
            }
        }
    }
    printf("%dx%d\n", max_loc.y, max_loc.x);
    for(int i = 0; i < brd_min->size; i++){
        free(res_board[i]);
        res_board[i] = NULL;
    }
    free(res_board);
    res_board = NULL;
    destroy_location_possibility(brd_min, loc_pos);
    destroy_board_min(brd_min, true);
    return max_loc;
}

void destroy_android(struct android *bot)
{
    destroy_board(bot->brd);
    free(bot);
}

void destroy_location_possibility(struct board_min *brd, int ****loc_pos)
{
    for(int i = 0; i < brd->afloat_ship_count; i++){
        for(int j = 0; j < brd->size; j++){
            for(int k = 0; k < brd->size; k++){
                free(loc_pos[i][j][k]);
                loc_pos[i][j][k] = NULL;
            }
            free(loc_pos[i][j]);
            loc_pos[i][j] = NULL;
        }
        free(loc_pos[i]);
        loc_pos[i] = NULL;
    }
    free(loc_pos);
    loc_pos = NULL;
}