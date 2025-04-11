#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>   // For text rendering
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

// Flash effect globals (for background flash on collision)
extern int flashBackground;
extern Uint32 flashStartTime;

// Global TTF font for rendering text
extern TTF_Font *font;

void save_game();
void load_game();

void init_game();
void update_game();
void cleanup_game();

// Render helper: displays a text string on screen at (x, y)
void render_text(SDL_Surface *screen, TTF_Font *font, const char *text, int x, int y);

#endif

