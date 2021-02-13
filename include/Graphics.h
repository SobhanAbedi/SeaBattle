//
// Created by Sobhan on 2/10/2021.
//

#ifndef SEABATTLE_GRAPHICS_H
#define SEABATTLE_GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "Player.h"
#include "Mechanism.h"

#define STOP_TIME 600

//struct declarations
struct window;
struct event_result;
struct asset;
struct page;

//function declarations
SDL_Texture *load_texture(char* file_path, SDL_Renderer *render_target);
void theme_to_path(char* path, int theme);
TTF_Font* load_font_bold(int size);
TTF_Font* load_font_regular(int size);
void load_asset(char *main_path, char *name, int texture_count, int theme, int w, int h, struct asset* dst);
bool init_graphics(int w, int h, int theme);
bool init_graphics(int w, int h, int theme);
struct asset* get_board_assets();
struct event_result menu_check_event(int start_height, int step, int x, SDL_Rect *button_rect, int cur_active_button);
int load_menu();
struct event_result name_check_event(SDL_Rect *button_rect, bool button_active);
bool get_name(char *name);
struct event_result save_name_check_event(SDL_Rect *button_rect, int active_button);
bool get_save_name(char *name);
struct event_result load_name_check_event(SDL_Rect *button_rect, bool button_active);
bool is_name_valid(char *name, char **list, int list_count);
bool get_load_name(char *name);
struct event_result draw_board_check_event(struct event_result *m_loc, struct event_result pre_res, double width, SDL_Rect *button_rect, bool is_single_click);
struct event_result draw_board(struct board *brd, struct player_simp *offensive, struct player_simp *defensive, bool is_res_map);
struct event_result place_ships_check_event(struct event_result *m_loc, struct event_result pre_res, double width, SDL_Rect *button_rect);
int place_ships(struct board *brd, struct config_ship_list *conf_ship_list, char *name);
struct event_result config_page_check_event(struct event_result pre_res, struct asset *button);
bool config_page();
bool show_msg(char *msg, int time);
//struct event_result check_event();
bool run_game();
bool close_graphics();

//struct definitions
struct window {
    SDL_Window *handle;
    SDL_Texture *texture_target;
    SDL_Renderer *renderer;
    int w, h;
    int type; //-1:closed, 0:loading page, 1:menu, 2:get player, 3:load game, 4:game board, 5:save game, 6:config page
};

struct event_result {
    int x1, x2;
    bool made_change;
};

struct asset{
    SDL_Texture **texture;
    SDL_Rect rect;
    int cur_texture;
};

struct page{
    struct asset background;
    int placeable_asset_count;
    struct asset *placeable_assets;
};


#endif //SEABATTLE_GRAPHICS_H
