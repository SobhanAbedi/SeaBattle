//
// Created by Sobhan on 2/10/2021.
//

#include "Graphics.h"

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

void load_asset(char *main_path, char *name, int theme, int w, int h, struct asset* dst)
{
    char temp_path[128];
    strcpy(temp_path, main_path);
    if(theme != -1)
        theme_to_path(name, theme);
    strcat(temp_path, name);
    (*dst).texture = load_texture(temp_path, main_window.renderer);
    (*dst).rect.x = 0;
    (*dst).rect.y = 0;
    (*dst).rect.w = w;
    (*dst).rect.h = h;
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



    //Loading pages. There will be 6 pages: 0:loading page, 1:menu, 2:get player, 3:load game, 4:game board, 5: save game, 6:config page
    pages = (struct page*)malloc(6 * sizeof(struct page));
    char main_path[128] = "../resources/graphics/png/";
    char name[128];

    //loading 0:loading page:
    strcpy(name, "ships.png");
    load_asset(main_path, name, -1, w, h, &pages[0].background);
    pages[0].placeable_asset_count = 0;
    pages[0].placeable_assets = NULL;

    //loading 1:menu
    strcpy(name, "menu.png");
    load_asset(main_path, name,0, w, h, &pages[1].background);
    pages[1].placeable_asset_count = 10;
    pages[1].placeable_assets = (struct asset*)malloc(pages[1].placeable_asset_count * sizeof(struct asset));
    for(int i = 0; i < pages[1].placeable_asset_count; i++){
        sprintf(name, "menu_%d.png", i);
        load_asset(main_path, name,0, 300, 100, &(pages[1].placeable_assets[i]));
    }

    //loading 2:get player

    //Showing Splash screen
    SDL_SetRenderDrawColor(main_window.renderer, 200, 200, 230, 255);
    SDL_RenderClear(main_window.renderer);

    //Drawing background
    SDL_RenderCopy(main_window.renderer, pages[0].background.texture,&(pages[0].background.rect), NULL);

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
        SDL_RenderCopy(main_window.renderer, pages[1].background.texture, &(pages[1].background.rect), NULL);

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
        int star_height = 180, step = 104;
        for (int i = 0; i < 5; i++) {
            dst_rect.y = star_height + step * i;
            int n = i * 2;
            if (i == active_button)
                n++;
            SDL_RenderCopy(main_window.renderer, pages[1].placeable_assets[n].texture,
                           &pages[1].placeable_assets[i].rect, &dst_rect);
        }
        SDL_SetRenderTarget(main_window.renderer, NULL);
        SDL_RenderCopy(main_window.renderer, main_window.texture_target, NULL, NULL);
        SDL_RenderPresent(main_window.renderer);
        res = menu_check_event(star_height, step, dst_rect.x, &(pages[1].placeable_assets[0].rect), active_button);
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
        switch (res.x2) {
            case 4:
                return -1;
            default:
                return 1;
        }
    }
    return 1;
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
    while(is_running){
        switch (main_window.type) {
            case -1:
                is_running = false;
            case 0:
                main_window.type = 1;
                break;
            case 1:
                main_window.type = load_menu();
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


