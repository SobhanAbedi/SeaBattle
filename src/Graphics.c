//
// Created by Sobhan on 2/10/2021.
//

#include "Graphics.h"

SDL_Window *main_window;

bool init_graphics(int x, int y)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("Video Initialization Error: %s\n", SDL_GetError());
        return NULL;
    }
    main_window = SDL_CreateWindow("SeaBattle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, x, y, SDL_WINDOW_SHOWN);
    if(main_window == NULL){
        printf("Window Creation Error: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Surface *window_surface = NULL, *image_surface = NULL;
    window_surface = SDL_GetWindowSurface(main_window);
    image_surface = SDL_LoadBMP("../resources/graphics/bmp/ships.bmp");
    if(image_surface == NULL){
        printf("Could not read file: %s\n", SDL_GetError());
        return -1;
    }
    SDL_BlitSurface(image_surface, NULL, window_surface, NULL);
    SDL_UpdateWindowSurface(main_window);
}

bool check_window()
{
    SDL_Event evnt;
    SDL_UpdateWindowSurface(main_window);
    while(!SDL_PollEvent(&evnt));
    if (evnt.type == SDL_QUIT)
        return false;
    return true;
}

bool close_graphics()
{
    SDL_DestroyWindow(main_window);
    SDL_Quit();
}


