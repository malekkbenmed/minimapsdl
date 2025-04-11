#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"

GameState game;

void init_game() {
    printf("Initializing SDL...\n");
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    printf("Initializing SDL_image...\n");
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image failed: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }

    printf("Creating window...\n");
    game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if(!game.screen) {
        fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        exit(1);
    }

    printf("Loading background...\n");
    SDL_Surface *temp = IMG_Load("assets/background.jpg");
    if(!temp) {
        fprintf(stderr, "Failed to load background: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    game.background = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!game.background) {
        fprintf(stderr, "Failed to convert background surface\n");
        cleanup_game();
        exit(1);
    }

    game.running = 1;
    printf("Game initialized successfully!\n");
}

void update_game() {
    // Clear screen with black color
    SDL_FillRect(game.screen, NULL, SDL_MapRGB(game.screen->format, 0, 0, 0));
    // Draw background
    SDL_BlitSurface(game.background, NULL, game.screen, NULL);
    // Draw objects, player and minimap (in that order)
    draw_objects();
    draw_player();
    draw_minimap();
    SDL_Flip(game.screen);
}

void cleanup_game() {
    printf("Cleaning up resources...\n");
    if(game.background)
        SDL_FreeSurface(game.background);
    IMG_Quit();
    SDL_Quit();
}

