#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "game.h"

typedef struct {
    SDL_Surface* sprite;
    SDL_Rect position;
    int active;      // 1 = visible/active, 0 = collected/inactive
    int frame;       // Current frame for coin animation
    int max_frame;   // Total frames (for coin sprite sheet; e.g., 4)
} GameObject;

extern GameObject platform;
extern GameObject coin;

int check_collision(SDL_Rect a, SDL_Rect b);
void init_objects();
void update_objects();
void draw_objects();

#endif

