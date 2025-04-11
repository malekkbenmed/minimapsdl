#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GROUND_LEVEL 500

typedef struct {
    SDL_Surface *screen;
    SDL_Surface *background;
    int running;
} GameState;

extern GameState game;

void init_game();
void update_game();
void cleanup_game();

#endif

