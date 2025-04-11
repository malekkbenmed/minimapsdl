#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"

GameState game;
int flashBackground = 0;
Uint32 flashStartTime = 0;
TTF_Font *font = NULL;  // Global font used for text

// Renders text using TTF_RenderText_Blended for a nice anti-aliased effect.
void render_text(SDL_Surface *screen, TTF_Font *font, const char *text, int x, int y) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, white);
    if (textSurface) {
        SDL_Rect dest = { x, y, 0, 0 };
        SDL_BlitSurface(textSurface, NULL, screen, &dest);
        SDL_FreeSurface(textSurface);
    }
}

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

    // Initialize TTF
    if(TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        exit(1);
    }

    // Load font (ensure assets/arial.ttf exists or change the path)
    font = TTF_OpenFont("assets/arial.ttf", 16);
    if(!font) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    printf("Creating window...\n");
    game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if(!game.screen) {
        fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
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

    flashBackground = 0;
    flashStartTime = 0;
    game.running = 1;
    printf("Game initialized successfully!\n");
}

void update_game() {
    // Clear screen with black first.
    SDL_FillRect(game.screen, NULL, SDL_MapRGB(game.screen->format, 0, 0, 0));

    // Flash effect: if flash is active and within 200ms, fill screen white.
    if (flashBackground) {
        if (SDL_GetTicks() - flashStartTime < 200) {
            SDL_FillRect(game.screen, NULL, SDL_MapRGB(game.screen->format, 255, 255, 255));
        } else {
            flashBackground = 0;
            SDL_BlitSurface(game.background, NULL, game.screen, NULL);
        }
    } else {
        SDL_BlitSurface(game.background, NULL, game.screen, NULL);
    }

    // Draw game objects, player, and minimap.
    draw_objects();
    draw_player();
    draw_minimap();

    // Render instructions at the bottom of the screen.
    render_text(game.screen, font, "Press S to Save | Press L to Load", 10, SCREEN_HEIGHT - 30);

    SDL_Flip(game.screen);
}

void cleanup_game() {
    printf("Cleaning up resources...\n");
    if(game.background)
        SDL_FreeSurface(game.background);
    if(font)
        TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void save_game() {
    FILE *fp = fopen("save.txt", "w");
    if(!fp) {
        fprintf(stderr, "Failed to open save file for writing\n");
        return;
    }
    // Save player's x, y and coin's active state.
    fprintf(fp, "%d %d %d\n", player.position.x, player.position.y, coin.active);
    fclose(fp);
    printf("Game saved!\nKYS!!!");
}

void load_game() {
    FILE *fp = fopen("save.txt", "r");
    if(!fp) {
        fprintf(stderr, "Failed to open save file for reading\n");
        return;
    }
    // Use %hd for player's position since they are of type Sint16.
    fscanf(fp, "%hd %hd %d", &player.position.x, &player.position.y, &coin.active);
    fclose(fp);
    printf("Game loaded!\nKYS!!!");
}

