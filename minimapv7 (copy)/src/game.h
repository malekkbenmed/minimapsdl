#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "player.h"

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  600
#define GROUND_LEVEL   534
#define MAX_HEALTH     100
#define MAX_LEVELS     6

typedef struct {
    SDL_Surface* screen;
    int          running;
    int          health;
    int          score;
} GameState;

extern GameState   game;
extern int         flashBackground;
extern Uint32      flashStartTime;
extern TTF_Font*   font;
extern SDL_Surface *sky, *city, *ground;
extern Player      player;
extern int         current_level;

void init_game();
void update_game();
void save_game();
void load_game();
void cleanup_game();
void render_text(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y);
void load_level(int level);

#endif
