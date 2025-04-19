#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "player.h"

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  600
#define GROUND_LEVEL   500
#define MAX_HEALTH     100

typedef struct {
    SDL_Surface* screen;
    int          running;
    int          health;   // current HP
    int          score;    // current score
} GameState;

// global game state
extern GameState   game;

// shared globals
extern int         flashBackground;
extern Uint32      flashStartTime;
extern TTF_Font*   font;
extern SDL_Surface *sky, *city, *ground;
extern Player      player;

// core functions
void init_game();
void update_game();
void save_game();
void load_game();
void cleanup_game();

// text renderer
void render_text(SDL_Surface* screen,
                 TTF_Font*      font,
                 const char*    text,
                 int            x,
                 int            y);

#endif // GAME_H

