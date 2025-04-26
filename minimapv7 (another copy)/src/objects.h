#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL/SDL.h>

typedef struct {
    SDL_Surface* sprite;
    SDL_Rect position;
    int active;
    int frame;
    int max_frame;
    int velocityX;
    int leftLimit;
    int rightLimit;
} GameObject;

extern GameObject platform;
extern GameObject coin;
extern GameObject obstacle;
extern SDL_Rect subwayEntry;

void init_objects();
void update_objects();
void draw_objects();
int check_collision(SDL_Rect a, SDL_Rect b);

#endif
