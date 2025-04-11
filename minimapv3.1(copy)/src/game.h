#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "player.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GROUND_LEVEL 470

typedef struct {
    SDL_Surface* screen;
    int running;
} GameState;

extern GameState game;
extern SDL_Surface *sky, *ground;
extern int flashBackground;
extern Uint32 flashStartTime;
extern TTF_Font* font;
extern Player player;  // Global player declaration

void save_game();
void load_game();
void init_game();
void update_game();
void cleanup_game();
void render_text(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y);

#endif
