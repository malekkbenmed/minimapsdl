#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

GameState game;
int flashBackground = 0;
Uint32 flashStartTime = 0;
TTF_Font* font = NULL;
SDL_Surface *sky = NULL;
SDL_Surface *city = NULL; 
SDL_Surface *ground = NULL;
Player player;

void render_text(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, white);
    if (textSurface) {
        SDL_Rect dest = {x, y, 0, 0};
        SDL_BlitSurface(textSurface, NULL, screen, &dest);
        SDL_FreeSurface(textSurface);
    }
}

void init_game() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image failed: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        exit(1);
    }

    font = TTF_OpenFont("assets/arial.ttf", 16);
    if (!font) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!game.screen) {
        fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    // Load sky background.
    SDL_Surface* temp = IMG_Load("assets/sky.jpg");
    if (!temp) {
        fprintf(stderr, "Failed to load sky: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    sky = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    // Load city background.
    temp = IMG_Load("assets/city.jpg");
    if (!temp) {
        fprintf(stderr, "Failed to load city background: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    city = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    // Load ground background.
    temp = IMG_Load("assets/ground.jpg");
    if (!temp) {
        fprintf(stderr, "Failed to load ground: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    ground = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    game.running = 1;
}

void update_game() {
    // Draw sky as the furthest background.
    SDL_BlitSurface(sky, NULL, game.screen, NULL);

    // Draw city background between sky and ground.
    SDL_BlitSurface(city, NULL, game.screen, NULL);

    // Draw ground at the front.
    SDL_Rect ground_pos = {0, GROUND_LEVEL, ground->w, ground->h};
    SDL_BlitSurface(ground, NULL, game.screen, &ground_pos);

    // Draw objects, player, minimap.
    draw_objects();
    draw_player(&player, game.screen);
    draw_minimap();

    // Draw save/load instructions.
    render_text(game.screen, font, "Press S to Save | Press L to Load", 10, SCREEN_HEIGHT - 30);

    SDL_Flip(game.screen);
}


void save_game() {
    FILE* fp = fopen("save.txt", "w");
    if (!fp) {
        fprintf(stderr, "Failed to open save file for writing\n");
        return;
    }
    fprintf(fp, "%d %d %d\n", player.position.x, player.position.y, coin.active);
    fclose(fp);
    printf("Game saved successfully!\n");
}

void load_game() {
    FILE* fp = fopen("save.txt", "r");
    if (!fp) {
        fprintf(stderr, "No save file found\n");
        return;
    }
    fscanf(fp, "%hd %hd %d", &player.position.x, &player.position.y, &coin.active);
    fclose(fp);
    printf("Game loaded successfully!\n");
}

void cleanup_game() {
    if (sky) SDL_FreeSurface(sky);
    if (city) SDL_FreeSurface(city);  // Free the city surface.
    if (ground) SDL_FreeSurface(ground);
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

