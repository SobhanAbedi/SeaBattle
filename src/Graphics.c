//
// Created by Sobhan on 2/10/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Graphics.h"
#include "Player.h"
#include "Mechanism.h"

struct window main_window;
struct page *pages;

SDL_Texture *load_texture(char* file_path, SDL_Renderer *render_target)
{
    SDL_Surface *surface = IMG_Load(file_path);
    if(surface == NULL){
        printf("Could not open texture: %s\n", file_path);
        SDL_FreeSurface(surface);
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(render_target, surface);
    if(texture == NULL)
        printf("Could not convert to texture: %s\n", file_path);
    SDL_FreeSurface(surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

void theme_to_path(char* path, int theme)
{
    char tmp_path[128];
    char prefix[][10] = {"light_", "dark_"};
    strcpy(tmp_path, path);
    strcpy(path, prefix[theme]);
    strcat(path, tmp_path);
    free(tmp_path);
}

void load_asset(char *main_path, char *name, int texture_count, int theme, int w, int h, struct asset* dst)
{
    char temp_path[128];
    if(theme != -1)
        theme_to_path(name, theme);
    (*dst).texture = (SDL_Texture**)malloc(texture_count * sizeof(SDL_Texture*));
    for(int i = 0; i < texture_count; i++) {
        sprintf(temp_path, "%s%s_%d.png", main_path, name, i);
        (*dst).texture[i] = load_texture(temp_path, main_window.renderer);
    }
    (*dst).rect.x = 0;
    (*dst).rect.y = 0;
    if(w == -1 || h == -1) {
        SDL_QueryTexture((*dst).texture[0], NULL, NULL, &((*dst).rect.w), &((*dst).rect.h));
    } else {
        (*dst).rect.w = w;
        (*dst).rect.h = h;
    }
    (*dst).cur_texture = 0;
}

TTF_Font* load_font_bold(int size)
{
    TTF_Font *font = TTF_OpenFont("../resources/fonts/OpenSans-SemiBold.ttf", size);
    if(font == NULL)
        printf("Could not open font\n");
    return font;
}

TTF_Font* load_font_regular(int size)
{
    TTF_Font *font = TTF_OpenFont("../resources/fonts/OpenSans-Regular.ttf", size);
    if(font == NULL)
        printf("Could not open font\n");
    return font;
}

bool init_graphics(int w, int h, int theme)
{
    printf("trying to init_graphics\n");
    //Initializing main_window
    main_window.w = w;
    main_window.h = h;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("Video Initialization Error: %s\n", SDL_GetError());
        return false;
    }
    if(IMG_Init(IMG_INIT_PNG) < 0){
        printf("PNG initialization Error %s\n", IMG_GetError());
        return false;
    }

    if(TTF_Init() < 0){
        printf("Font initialization Error: %s\n", TTF_GetError());
        return false;
    }

    main_window.handle = SDL_CreateWindow("SeaBattle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, main_window.w, main_window.h, SDL_WINDOW_SHOWN);
    main_window.type = 0;
    if(main_window.handle == NULL){
        printf("Window Creation Error: %s\n", SDL_GetError());
        return false;
    }
    main_window.renderer = SDL_CreateRenderer(main_window.handle, -1, SDL_RENDERER_ACCELERATED);

    printf("initialized everything. trying to load assets\n");

    //Loading pages. There will be 7 pages: 0:loading page, 1:menu, 2:get player, 3:load game, 4:game board, 5:save game, 6:config page
    pages = (struct page*)malloc(7 * sizeof(struct page));
    char main_path[128] = "../resources/graphics/png/";
    char name[128];

    //loading 0:loading page:
    strcpy(name, "ships_w");
    load_asset(main_path, name, 1, -1, w, h, &pages[0].background);
    pages[0].placeable_asset_count = 0;
    pages[0].placeable_assets = NULL;
    printf("page 0 loaded\n");
    //loading 1:menu
    strcpy(name, "menu_w");
    load_asset(main_path, name, 1, 0, w, h, &pages[1].background);
    pages[1].placeable_asset_count = 5;
    pages[1].placeable_assets = (struct asset*)malloc(pages[1].placeable_asset_count * sizeof(struct asset));
    for(int i = 0; i < pages[1].placeable_asset_count; i++){
        sprintf(name, "menu_%d", i);
        load_asset(main_path, name, 2, 0, 300, 100, &(pages[1].placeable_assets[i]));
    }
    printf("page 1 loaded\n");
    //loading 2:get player
    pages[2].background.texture = NULL;
    pages[2].placeable_asset_count = 1;
    pages[2].placeable_assets = (struct asset*)malloc(pages[2].placeable_asset_count * sizeof(struct asset));
    for(int i = 0; i < pages[2].placeable_asset_count; i++){
        sprintf(name, "name_%d", i);
        load_asset(main_path, name, 3, -1, -1, -1, &(pages[2].placeable_assets[i]));
    }
    printf("page 2 loaded\n");
    //loading 3:load game
    pages[3].background.texture = NULL;
    pages[3].placeable_asset_count = 1;
    pages[3].placeable_assets = (struct asset*)malloc(pages[3].placeable_asset_count * sizeof(struct asset));
    for(int i = 0; i < pages[3].placeable_asset_count; i++) {
        sprintf(name, "load_%d", i);
        load_asset(main_path, name, 3, -1, -1, -1, &(pages[3].placeable_assets[i]));
    }
    printf("page 3 loaded\n");
    //loading 4:game board
    strcpy(name, "board_w");
    load_asset(main_path, name, 1, -1, w, h, &pages[4].background);
    pages[4].placeable_asset_count = 11;
    pages[4].placeable_assets = (struct asset*)malloc(pages[4].placeable_asset_count * sizeof(struct asset));
    for(int i = 0; i < pages[4].placeable_asset_count; i++){
        sprintf(name, "board_%d", i);
        load_asset(main_path, name, 1,  -1, -1, -1, &(pages[4].placeable_assets[i]));
    }
    printf("page 4 loaded\n");
    //loading 5: save game
    pages[5].background.texture = NULL;
    pages[5].placeable_asset_count = 2;
    pages[5].placeable_assets = (struct asset*)malloc(pages[5].placeable_asset_count * sizeof(struct asset));
    int n[2] = {2, 3};
    for(int i = 0; i < pages[5].placeable_asset_count; i++) {
        sprintf(name, "save_%d", i);
        load_asset(main_path, name, n[i], -1, -1, -1, &(pages[5].placeable_assets[i]));
    }
    printf("page 5 loaded\n");
    //loading 6:config page

    printf("loaded Completely\n");

    //Showing Splash screen
    SDL_SetRenderDrawColor(main_window.renderer, 200, 200, 230, 255);
    SDL_RenderClear(main_window.renderer);

    //Drawing background
    SDL_RenderCopy(main_window.renderer, pages[0].background.texture[0],&(pages[0].background.rect), NULL);

    //Writing some text
    TTF_Font *font = load_font_bold(36);
    SDL_Color text_color = {32, 32, 32, 255};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, "POWERED BY SDL2", text_color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
    SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
    SDL_Rect text_rect;
    text_rect.x = 950;
    text_rect.y = 675;
    SDL_QueryTexture(text_texture, NULL, NULL, &text_rect.w, &text_rect.h);
    SDL_RenderCopy(main_window.renderer, text_texture, NULL, &text_rect);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    text_texture = NULL;
    text_surface = NULL;

    //presenting the result
    SDL_RenderPresent(main_window.renderer);
    SDL_Delay(1000);
    return true;
}

struct asset* get_board_assets()
{
    return pages[4].placeable_assets;
}

struct event_result menu_check_event(int start_height, int step, int x, SDL_Rect *button_rect, int cur_active_button)
{
    //if(evnt.type == SDL_MOUSEMOTION)
    //    printf("%d, %d\n", evnt.motion.x, evnt.motion.y);
    SDL_Event evnt;
    struct event_result res = {0, 0, false};
    while (!res.made_change) {
        while (!SDL_PollEvent(&evnt))
            SDL_Delay(10);
        if (evnt.type == SDL_QUIT) {
            res.x1 = res.x2 = -1;
            res.made_change = true;
            break;
        } else if (evnt.type == SDL_MOUSEMOTION) {
            bool still_active = false;
            for (int i = 0; i < 5; i++) {
                if (evnt.motion.x >= x && evnt.motion.x <= x + button_rect->w &&
                    evnt.motion.y > start_height + i * step &&
                    evnt.motion.y < start_height + i * step + button_rect->h) {
                    if(i == cur_active_button){
                        still_active = true;
                        continue;
                    }
                    res.x1 = 1;
                    res.x2 = i;
                    res.made_change = true;
                    return res;
                }
            }
            if(!still_active && cur_active_button != -1)
                return res;
        } else if (evnt.type == SDL_MOUSEBUTTONDOWN && evnt.button.button == SDL_BUTTON_LEFT) {
            for (int i = 0; i < 5; i++) {
                if (evnt.button.x >= x && evnt.button.x <= x + button_rect->w &&
                    evnt.button.y > start_height + i * step &&
                    evnt.button.y < start_height + i * step + button_rect->h) {
                    res.x1 = 2;
                    res.x2 = i;
                    res.made_change = true;
                    return res;
                }
            }
        }
    }
    return res;
}

int load_menu()
{
    char **list = get_top5_players();
    int top_player_count = 0;
    for(int i = 0; i < 5 && list != NULL && list[i][0] != 0; i++){
        top_player_count++;
    }
    int active_button = -1;
    struct event_result res;
    TTF_Font *font = load_font_bold(36);
    SDL_Color back_color = {0, 150, 150, 150};
    SDL_Color text_color = {255, 255, 255, 255};
    //SDL_Surface *text_surface = TTF_RenderText_Shaded(font, "TOP 5 PLAYERS:\nSobhan", text_color, back_color);
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, "TOP 5 PLAYERS:", text_color);
    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
    SDL_SetTextureBlendMode(title_texture, SDL_BLENDMODE_BLEND);
    SDL_Rect text_rect;
    text_rect.x = 800;
    SDL_Rect text_lim = {0, 0, 410, 0};

    free(font);
    font = load_font_regular(24);
    SDL_Texture **list_texture;
    if(top_player_count == 0) {
        list_texture = (SDL_Texture**)malloc(1 * sizeof(SDL_Texture*));
        text_surface = TTF_RenderText_Blended(font, "There are no players yet", text_color);
        list_texture[0] = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
    } else {
        list_texture = (SDL_Texture**)malloc(top_player_count * sizeof(SDL_Texture*));
        for(int i = 0; i < top_player_count; i++){
            text_surface = TTF_RenderText_Blended(font, list[i], text_color);
            list_texture[i] = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
        }
    }
    SDL_Rect text_box = {780, 90, 450, 500};
    //printf("%dx%d\n", text_rect.w, text_rect.h);
    //SDL_RenderCopy(main_window.renderer, title_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    text_surface = NULL;

    do {
        main_window.texture_target = SDL_CreateTexture(main_window.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, main_window.w, main_window.h);
        SDL_SetTextureBlendMode(main_window.texture_target, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(main_window.renderer, main_window.texture_target);
        SDL_SetRenderDrawColor(main_window.renderer, 0, 0, 0, 0);
        SDL_RenderClear(main_window.renderer);
        SDL_RenderCopy(main_window.renderer, pages[1].background.texture[0], &(pages[1].background.rect), NULL);

        SDL_SetRenderDrawColor(main_window.renderer, 0, 150, 150, 255);
        SDL_RenderFillRect(main_window.renderer, &text_box);
        SDL_SetRenderDrawColor(main_window.renderer, 0, 0, 0, 255);
        SDL_QueryTexture(title_texture, NULL, NULL, &text_rect.w, &text_rect.h);
        if(text_rect.w > 410)
            text_rect.w = 410;
        text_lim.h = text_rect.h;
        text_rect.y = 100;
        SDL_RenderCopy(main_window.renderer, title_texture, &text_lim, &text_rect);
        text_rect.y += 100;
        if(top_player_count == 0)
            top_player_count++;
        for(int i = 0; i < top_player_count; i++){
            SDL_QueryTexture(list_texture[i], NULL, NULL, &text_rect.w, &text_rect.h);
            if(text_rect.w > 410)
                text_rect.w = 410;
            text_lim.h = text_rect.h;
            SDL_RenderCopy(main_window.renderer, list_texture[i], &text_lim, &text_rect);
            text_rect.y += 50;
        }
        SDL_Rect dst_rect = {300, 0, 300, 100};
        int start_height = 180, step = 104;
        for (int i = 0; i < 5; i++) {
            dst_rect.y = start_height + step * i;

            if (i == active_button) {
                pages[1].placeable_assets[i].cur_texture = 1;
            } else {
                pages[1].placeable_assets[i].cur_texture = 0;
            }
            SDL_RenderCopy(main_window.renderer, pages[1].placeable_assets[i].texture[pages[1].placeable_assets[i].cur_texture],
                           &pages[1].placeable_assets[i].rect, &dst_rect);
        }
        SDL_SetRenderTarget(main_window.renderer, NULL);
        SDL_RenderCopy(main_window.renderer, main_window.texture_target, NULL, NULL);
        SDL_RenderPresent(main_window.renderer);
        res = menu_check_event(start_height, step, dst_rect.x, &(pages[1].placeable_assets[0].rect), active_button);
        //printf("%d %d\n", res.x1, res.x2);
        if(res.x1 == 1)
            active_button = res.x2;
        else if(res.x1 == 0)
            active_button = -1;

    } while(res.x1 == 0 || res.x1 == 1);

    SDL_DestroyTexture(title_texture);
    title_texture = NULL;

    for(int i = 0; i < 5 && list != NULL; i++){
        free(list[i]);
        list[i] = NULL;
    }
    free(list);
    list = NULL;

    printf("%d %d\n", res.x1, res.x2);
    if(res.x1 == -1 && res.x2 == -1)
        return -1;
    if(res.x1 == 2) {
        return res.x2;
    }
    return 1;
}

struct event_result name_check_event(SDL_Rect *button_rect, bool button_active)
{
    //if(evnt.type == SDL_MOUSEMOTION)
    //    printf("%d, %d\n", evnt.motion.x, evnt.motion.y);
    SDL_Event evnt;
    struct event_result res = {0, 0, false};
    while (!res.made_change) {
        while (!SDL_PollEvent(&evnt))
            SDL_Delay(10);
        if (evnt.type == SDL_QUIT) {
            res.x1 = res.x2 = -1;
            res.made_change = true;
            break;
        } else if (evnt.type == SDL_MOUSEMOTION) {
            bool still_active = false;
            if (evnt.motion.x >= button_rect->x && evnt.motion.x <= button_rect->x + button_rect->w &&
                evnt.motion.y >= button_rect->y && evnt.motion.y <= button_rect->y + button_rect->h) {
                if(button_active){
                    still_active = true;
                }else {
                    res.x1 = 1;
                    res.made_change = true;
                    return res;
                }
            }

            if(button_active && !still_active) {
                res.made_change = true;
                return res;
            }
        } else if (evnt.type == SDL_MOUSEBUTTONDOWN && evnt.button.button == SDL_BUTTON_LEFT) {
            if (evnt.button.x >= button_rect->x && evnt.button.x <= button_rect->x + button_rect->w &&
                evnt.button.y >= button_rect->y && evnt.button.y <= button_rect->y + button_rect->h) {
                res.x1 = 2;
                res.made_change = true;
                return res;
            }
        } else if (evnt.type == SDL_TEXTINPUT){
            res.x1 = 3;
            res.x2 = (int)(evnt.text.text[0]);
            res.made_change = true;
        } else if(evnt.type == SDL_KEYDOWN && evnt.key.keysym.sym == SDLK_BACKSPACE){
            res.x1 = 3;
            res.x2 = -1;
            res.made_change = true;
        } else if(evnt.type == SDL_KEYDOWN && (evnt.key.keysym.sym == SDLK_KP_ENTER || evnt.key.keysym.sym == SDLK_RETURN)){
            res.x1 = 2;
            res.made_change = true;
        }
    }
    return res;
}

bool get_name(char *name)
{
    main_window.type = 2;

    //Writing some text
    TTF_Font *font = load_font_bold(36);
    SDL_Color text_color = {200 , 200, 200, 255};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, "Enter Your Name", text_color);
    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
    SDL_SetTextureBlendMode(title_texture, SDL_BLENDMODE_BLEND);
    SDL_Rect title_rect;
    title_rect.y = 250;
    SDL_QueryTexture(title_texture, NULL, NULL, &title_rect.w, &title_rect.h);
    title_rect.x = 640 - (title_rect.w / 2);

    SDL_FreeSurface(text_surface);
    text_surface = NULL;
    free(font);

    font = load_font_regular(24);
    struct SDL_Texture *name_texture;
    SDL_Rect name_rect = {360, 0, 0, 0};
    SDL_Rect text_box = {350, 300, 580, 70};
    SDL_Color back_color = {0 , 100, 100, 160};


    pages[2].placeable_assets[0].rect.x = 640 - (pages[2].placeable_assets[0].rect.w / 2);
    pages[2].placeable_assets[0].rect.y = 400;

    bool button_active = false;
    struct event_result res;
    //int n;
    do{
        SDL_SetRenderDrawColor(main_window.renderer, 120, 120, 130, 255);
        SDL_RenderClear(main_window.renderer);

        SDL_RenderCopy(main_window.renderer, title_texture, NULL, &title_rect);

        SDL_SetRenderDrawColor(main_window.renderer, 0, 100, 100, 255);
        SDL_RenderFillRect(main_window.renderer, &text_box);
        SDL_SetRenderDrawColor(main_window.renderer, 0, 0, 0, 255);

        text_surface = TTF_RenderText_Shaded(font, name, text_color, back_color);
        name_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
        SDL_QueryTexture(name_texture, NULL, NULL, &name_rect.w, &name_rect.h);
        name_rect.y = text_box.y + (text_box.h - name_rect.h) / 2;
        SDL_RenderCopy(main_window.renderer, name_texture, NULL, &name_rect);

        if(button_active)
            pages[2].placeable_assets[0].cur_texture = 1;
        else
            pages[2].placeable_assets[0].cur_texture = 0;
        if(strlen(name) < 3 || strlen(name) >= 16)
            pages[2].placeable_assets[0].cur_texture = 2;
        //printf("%d: %d %d %d %d\n", n, pages[2].placeable_assets[n].rect.x, pages[2].placeable_assets[n].rect.y, pages[2].placeable_assets[n].rect.w, pages[2].placeable_assets[n].rect.h);

        SDL_RenderCopy(main_window.renderer, pages[2].placeable_assets[0].texture[pages[2].placeable_assets[0].cur_texture], NULL, &(pages[2].placeable_assets[0].rect));
        SDL_RenderPresent(main_window.renderer);
        res = name_check_event(&(pages[2].placeable_assets[0].rect), button_active);
        //printf("%d %d\n", res.x1, res.x2);
        if(res.x1 == 1)
            button_active = true;
        else if(res.x1 == 0)
            button_active = false;
        else if(res.x1 == 3) {
            int size = strlen(name);
            if(res.x2 == -1){
                if(size > 0) {
                    name[size - 1] = 0;
                }
            }else if(size < 16){
                name[size++] = (char)res.x2;
                name[size] = 0;
            }
        }
    } while(res.x1 == 0 || res.x1 == 1 || res.x1 == 3 || strlen(name) < 3);

    SDL_FreeSurface(text_surface);
    text_surface = NULL;
    free(font);
    SDL_DestroyTexture(title_texture);
    title_texture = NULL;
    if(res.x1 == -1)
        return false;
    if(res.x1 == 2)
        return true;

}

struct event_result save_name_check_event(SDL_Rect *button_rect, int active_button)
{
    //if(evnt.type == SDL_MOUSEMOTION)
    //    printf("%d, %d\n", evnt.motion.x, evnt.motion.y);
    SDL_Event evnt;
    struct event_result res = {0, 0, false};
    int dx = button_rect->w + 20;
    while (!res.made_change) {
        while (!SDL_PollEvent(&evnt))
            SDL_Delay(10);
        if (evnt.type == SDL_QUIT) {
            res.x1 = res.x2 = -1;
            res.made_change = true;
            break;
        } else if (evnt.type == SDL_MOUSEMOTION) {
            bool still_active = false;
            for(int i = 0; i < 2; i++) {
                if (evnt.motion.x >= (button_rect->x + i * dx) && evnt.motion.x <= (button_rect->x + button_rect->w + i * dx) &&
                    evnt.motion.y >= button_rect->y && evnt.motion.y <= button_rect->y + button_rect->h) {
                    if (active_button == i) {
                        still_active = true;
                        break;
                    } else {
                        res.x1 = 1;
                        res.x2 = i;
                        res.made_change = true;
                        return res;
                    }
                }
            }
            if(active_button != -1 && !still_active) {
                res.made_change = true;
                return res;
            }
        } else if (evnt.type == SDL_MOUSEBUTTONDOWN && evnt.button.button == SDL_BUTTON_LEFT) {
            for(int i = 0; i < 2; i++) {
                if (evnt.button.x >= (button_rect->x + i * dx) && evnt.button.x <= (button_rect->x + button_rect->w + i * dx) &&
                    evnt.button.y >= button_rect->y && evnt.button.y <= button_rect->y + button_rect->h) {
                    res.x1 = 2;
                    res.x2 = i;
                    res.made_change = true;
                    return res;
                }
            }
        } else if (evnt.type == SDL_TEXTINPUT){
            res.x1 = 3;
            res.x2 = (int)(evnt.text.text[0]);
            res.made_change = true;
        } else if(evnt.type == SDL_KEYDOWN && evnt.key.keysym.sym == SDLK_BACKSPACE){
            res.x1 = 3;
            res.x2 = -1;
            res.made_change = true;
        } else if(evnt.type == SDL_KEYDOWN && (evnt.key.keysym.sym == SDLK_KP_ENTER || evnt.key.keysym.sym == SDLK_RETURN)){
            res.x1 = 2;
            res.x2 = 1;
            res.made_change = true;
        }
    }
    return res;
}

bool get_save_name(char *name)
{
    main_window.type = 5;

    //Writing some text
    TTF_Font *font = load_font_bold(36);
    SDL_Color text_color = {200 , 200, 200, 255};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, "Enter Save Name", text_color);
    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
    SDL_SetTextureBlendMode(title_texture, SDL_BLENDMODE_BLEND);
    SDL_Rect title_rect;
    title_rect.y = 250;
    SDL_QueryTexture(title_texture, NULL, NULL, &title_rect.w, &title_rect.h);
    title_rect.x = 640 - (title_rect.w / 2);

    SDL_FreeSurface(text_surface);
    text_surface = NULL;
    free(font);

    font = load_font_regular(24);
    struct SDL_Texture *name_texture;
    SDL_Rect name_rect = {360, 0, 0, 0};
    SDL_Rect text_box = {350, 300, 580, 70};
    SDL_Color back_color = {0 , 100, 100, 160};


    pages[5].placeable_assets[0].rect.x = 640 - (pages[5].placeable_assets[0].rect.w + 10);
    pages[5].placeable_assets[0].rect.y = 400;


    pages[5].placeable_assets[1].rect.x = 650;
    pages[5].placeable_assets[1].rect.y = 400;

    int active_button = -1;
    struct event_result res;
    //int n;
    do{
        SDL_SetRenderDrawColor(main_window.renderer, 120, 120, 130, 255);
        SDL_RenderClear(main_window.renderer);

        SDL_RenderCopy(main_window.renderer, title_texture, NULL, &title_rect);

        SDL_SetRenderDrawColor(main_window.renderer, 0, 100, 100, 255);
        SDL_RenderFillRect(main_window.renderer, &text_box);
        SDL_SetRenderDrawColor(main_window.renderer, 0, 0, 0, 255);

        text_surface = TTF_RenderText_Shaded(font, name, text_color, back_color);
        name_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
        SDL_QueryTexture(name_texture, NULL, NULL, &name_rect.w, &name_rect.h);
        name_rect.y = text_box.y + (text_box.h - name_rect.h) / 2;
        SDL_RenderCopy(main_window.renderer, name_texture, NULL, &name_rect);

        for(int i = 0; i < 2; i++){
            if(active_button == i)
                pages[5].placeable_assets[i].cur_texture = 1;
            else
                pages[5].placeable_assets[i].cur_texture = 0;
            if(i == 1 && (strlen(name) < 3 || strlen(name) >= 16))
                pages[5].placeable_assets[i].cur_texture = 2;
            SDL_RenderCopy(main_window.renderer, pages[5].placeable_assets[i].texture[pages[5].placeable_assets[i].cur_texture], NULL, &(pages[5].placeable_assets[i].rect));
        }
        //printf("%d: %d %d %d %d\n", n, pages[2].placeable_assets[n].rect.x, pages[2].placeable_assets[n].rect.y, pages[2].placeable_assets[n].rect.w, pages[2].placeable_assets[n].rect.h);

        SDL_RenderPresent(main_window.renderer);
        res = save_name_check_event(&(pages[5].placeable_assets[0].rect), active_button);
        //printf("%d %d\n", res.x1, res.x2);
        if(res.x1 == 1)
            active_button = res.x2;
        else if(res.x1 == 0)
            active_button = -1;
        else if(res.x1 == 3) {
            int size = strlen(name);
            if(res.x2 == -1){
                if(size > 0) {
                    name[size - 1] = 0;
                }
            }else if(size < 16){
                name[size++] = (char)res.x2;
                name[size] = 0;
            }
        }
    } while(res.x1 == 0 || res.x1 == 1 || res.x1 == 3 || strlen(name) < 3);

    SDL_FreeSurface(text_surface);
    text_surface = NULL;
    free(font);
    SDL_DestroyTexture(title_texture);
    title_texture = NULL;

    if(res.x1 == -1)
        return false;
    if(res.x1 == 2) {
        if(res.x2 == 0)
            name[0] = 0;
        return true;
    }
}

struct event_result load_name_check_event(SDL_Rect *button_rect, bool button_active)
{
    //if(evnt.type == SDL_MOUSEMOTION)
    //    printf("%d, %d\n", evnt.motion.x, evnt.motion.y);
    SDL_Event evnt;
    struct event_result res = {0, 0, false};
    while (!res.made_change) {
        while (!SDL_PollEvent(&evnt))
            SDL_Delay(10);
        if (evnt.type == SDL_QUIT) {
            res.x1 = res.x2 = -1;
            res.made_change = true;
            break;
        } else if (evnt.type == SDL_MOUSEMOTION) {
            bool still_active = false;
            if (evnt.motion.x >= button_rect->x && evnt.motion.x <= button_rect->x + button_rect->w &&
                evnt.motion.y >= button_rect->y && evnt.motion.y <= button_rect->y + button_rect->h) {
                if(button_active){
                    still_active = true;
                }else {
                    res.x1 = 1;
                    res.made_change = true;
                    return res;
                }
            }

            if(button_active && !still_active) {
                res.made_change = true;
                return res;
            }
        } else if (evnt.type == SDL_MOUSEBUTTONDOWN && evnt.button.button == SDL_BUTTON_LEFT) {
            if (evnt.button.x >= button_rect->x && evnt.button.x <= button_rect->x + button_rect->w &&
                evnt.button.y >= button_rect->y && evnt.button.y <= button_rect->y + button_rect->h) {
                res.x1 = 2;
                res.made_change = true;
                return res;
            }
        } else if (evnt.type == SDL_TEXTINPUT){
            res.x1 = 3;
            res.x2 = (int)(evnt.text.text[0]);
            res.made_change = true;
        } else if(evnt.type == SDL_KEYDOWN && evnt.key.keysym.sym == SDLK_BACKSPACE){
            res.x1 = 3;
            res.x2 = -1;
            res.made_change = true;
        } else if(evnt.type == SDL_KEYDOWN && (evnt.key.keysym.sym == SDLK_KP_ENTER || evnt.key.keysym.sym == SDLK_RETURN)){
            res.x1 = 2;
            res.made_change = true;
        }
    }
    return res;
}

bool is_name_valid(char *name, char **list, int list_count)
{
    for(int i = 0; i < list_count; i++) {
        if (strcmp(name, list[i]) == 0)
            return true;
    }
    return false;
}

bool get_load_name(char *name)
{
    main_window.type = 3;
    char **name_list = (char**)malloc(10 * sizeof(char*));
    for(int i = 0; i < 10; i++)
        name_list[i] = (char*)malloc(NAME_LEN * sizeof(char));
    //printf("trying to get saves\n");
    int saves_count = get_saves(name_list);
    //printf("got saves\n");
    if(saves_count == 0) {
        name[0] = 0;
        return true;
    }
    strcpy(name, name_list[0]);
    //Writing some text
    TTF_Font *font = load_font_bold(36);
    SDL_Color text_color = {200 , 200, 200, 255};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, "Enter Save Name", text_color);
    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
    SDL_SetTextureBlendMode(title_texture, SDL_BLENDMODE_BLEND);
    SDL_Rect title_rect;
    title_rect.y = 100;
    SDL_QueryTexture(title_texture, NULL, NULL, &title_rect.w, &title_rect.h);
    title_rect.x = 640 - (title_rect.w / 2);

    SDL_FreeSurface(text_surface);
    text_surface = NULL;
    free(font);

    font = load_font_regular(24);
    SDL_Texture *name_texture;
    SDL_Rect name_rect = {360, 0, 0, 0};
    SDL_Rect text_box1 = {350, 150, 580, 70};
    SDL_Rect text_box2 = {350, 240, 580, 340};
    SDL_Color back_color = {0 , 100, 100, 160};

    SDL_Texture **list_texture = (SDL_Texture**)malloc(saves_count * sizeof(SDL_Texture*));
    SDL_Rect list_rect[saves_count];
    int acc_height = 250;
    for(int i = 0; i < saves_count; i++){
        text_surface = TTF_RenderText_Shaded(font, name_list[i], text_color, back_color);
        list_texture[i] = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
        list_rect[i].x = 360;
        list_rect[i].y = acc_height;
        SDL_QueryTexture(list_texture[i], NULL, NULL, &(list_rect[i].w), &(list_rect[i].h));
        //printf("%d %d", list_rect[i].w, list_rect[i].h);
        acc_height += list_rect[i].h + 5;
    }

    pages[3].placeable_assets[0].rect.x = 640 - (pages[3].placeable_assets[0].rect.w / 2);
    pages[3].placeable_assets[0].rect.y = 600;

    bool button_active = false;
    struct event_result res;
    //int n;
    do{
        SDL_SetRenderDrawColor(main_window.renderer, 120, 120, 130, 255);
        SDL_RenderClear(main_window.renderer);

        SDL_RenderCopy(main_window.renderer, title_texture, NULL, &title_rect);

        SDL_SetRenderDrawColor(main_window.renderer, 0, 100, 100, 255);
        SDL_RenderFillRect(main_window.renderer, &text_box1);
        SDL_RenderFillRect(main_window.renderer, &text_box2);
        SDL_SetRenderDrawColor(main_window.renderer, 0, 0, 0, 255);

        text_surface = TTF_RenderText_Shaded(font, name, text_color, back_color);
        name_texture = SDL_CreateTextureFromSurface(main_window.renderer, text_surface);
        SDL_QueryTexture(name_texture, NULL, NULL, &name_rect.w, &name_rect.h);
        name_rect.y = text_box1.y + (text_box1.h - name_rect.h) / 2;
        SDL_RenderCopy(main_window.renderer, name_texture, NULL, &name_rect);
        for(int i = 0; i < saves_count; i++)
            SDL_RenderCopy(main_window.renderer, list_texture[i], NULL, &(list_rect[i]));

        if(button_active)
            pages[3].placeable_assets[0].cur_texture = 1;
        else
            pages[3].placeable_assets[0].cur_texture = 0;
        if(strlen(name) < 3 || strlen(name) >= 16 || !is_name_valid(name, name_list, saves_count))
            pages[3].placeable_assets[0].cur_texture = 2;
        //printf("%d: %d %d %d %d\n", n, pages[2].placeable_assets[n].rect.x, pages[2].placeable_assets[n].rect.y, pages[2].placeable_assets[n].rect.w, pages[2].placeable_assets[n].rect.h);

        SDL_RenderCopy(main_window.renderer, pages[3].placeable_assets[0].texture[pages[3].placeable_assets[0].cur_texture], NULL, &(pages[3].placeable_assets[0].rect));
        SDL_RenderPresent(main_window.renderer);
        res = load_name_check_event(&(pages[3].placeable_assets[0].rect), button_active);
        //printf("%d %d\n", res.x1, res.x2);
        if(res.x1 == 1)
            button_active = true;
        else if(res.x1 == 0)
            button_active = false;
        else if(res.x1 == 3) {
            int size = strlen(name);
            if(res.x2 == -1){
                if(size > 0) {
                    name[size - 1] = 0;
                }
            }else if(size < 16){
                name[size++] = (char)res.x2;
                name[size] = 0;
            }
        }
    } while(res.x1 == 0 || res.x1 == 1 || res.x1 == 3 || strlen(name) < 3 || !is_name_valid(name, name_list, saves_count));

    SDL_FreeSurface(text_surface);
    text_surface = NULL;
    free(font);
    SDL_DestroyTexture(title_texture);
    title_texture = NULL;
    if(res.x1 == -1)
        return false;
    if(res.x1 == 2)
        return true;

}


/*
struct event_result check_event()
{
    SDL_Event evnt;
    struct event_result res = {0, 0, false};
    while (!res.made_change){
        while(!SDL_PollEvent(&evnt))
            SDL_Delay(10);
        if (evnt.type == SDL_QUIT) {
            res.x1 = res.x2 = -1;
            res.made_change = true;
            break;
        }
        switch (main_window.type) {
            case 1:
                res = menu_check_event(evnt);
                break;
        }
    }
    return res;
}
*/

bool run_game()
{
    bool is_running = true;
    int res, run_res;
    while(is_running){
        switch (main_window.type) {
            case -1:
                is_running = false;
            case 0:
                main_window.type = 1;
                break;
            case 1:
                res = load_menu();
                switch (res) {
                    case 0:
                        if(!init_game_pvb())
                            main_window.type = -1;
                        else
                            main_window.type = 1;
                        break;
                    case 1:
                        if(!init_game_pvp())
                            main_window.type = -1;
                        else
                            main_window.type = 1;
                        break;
                    case 2:
                        if(!load_game())
                            main_window.type = -1;
                        else
                            main_window.type = 1;
                        break;
                    case 4:
                        main_window.type = -1;
                        break;
                    default:
                        main_window.type = 1;
                }
                break;
            default:
                printf("Unknown page requested\n");
                is_running = false;
                break;
        }
    }
}

bool close_graphics()
{
    SDL_DestroyWindow(main_window.handle);
    SDL_DestroyTexture(main_window.texture_target);
    SDL_DestroyRenderer(main_window.renderer);
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}


