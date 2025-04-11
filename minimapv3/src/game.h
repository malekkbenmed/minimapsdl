#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "player.h"

// Screen and ground definitions.
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GROUND_LEVEL 470  // Y coordinate where the ground begins

typedef struct {
    SDL_Surface* screen;
    int running;
} GameState;

extern GameState game;

// Background surfaces.
extern SDL_Surface *sky;
extern SDL_Surface *ground;

// Flash effect globals (for coin collection glow).
extern int flashBackground;
extern Uint32 flashStartTime;

// Global TTF font (for on-screen text).
extern TTF_Font* font;

// The global player (defined in player.h).
extern Player player;

// Save/Load function prototypes.
void save_game();
void load_game();

// Game lifecycle functions.
void init_game();
void update_game();
void cleanup_game();

// Render helper to draw text.
void render_text(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y);

#endif
