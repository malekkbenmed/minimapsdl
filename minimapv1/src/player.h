#ifndef PLAYER_H
#define PLAYER_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "game.h"

#define GRAVITY 1
#define JUMP_FORCE -25

typedef struct {
    SDL_Surface *sprite;
    SDL_Rect position;
    int velocity_y;
    int grounded;  // 1 if the player is on the ground/platform, 0 otherwise.
} Player;

extern Player player;

void init_player();
void update_player();
void draw_player();

#endif

