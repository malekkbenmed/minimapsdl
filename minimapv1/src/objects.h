#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "game.h"

typedef struct {
    SDL_Surface *sprite;
    SDL_Rect position;
    int active;  // flag indicating whether the object should be drawn/updated
} GameObject;

extern GameObject platform;
extern GameObject coin;

int check_collision(SDL_Rect a, SDL_Rect b);
void init_objects();
void update_objects();
void draw_objects();

#endif

