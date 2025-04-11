#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"

GameState game;
int flashBackground = 0;
Uint32 flashStartTime = 0;
TTF_Font *font = NULL;
SDL_Surface *sky = NULL;
SDL_Surface *ground = NULL;

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
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    printf("Initializing SDL_image...\n");
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image failed: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }

    printf("Initializing SDL_ttf...\n");
    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        exit(1);
    }

    // Load font (make sure assets/arial.ttf exists)
    font = TTF_OpenFont("assets/arial.ttf", 16);
    if (!font) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    printf("Creating window...\n");
    game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!game.screen) {
        fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    // Load the sky background.
    SDL_Surface *temp = IMG_Load("assets/sky.png");
    if (!temp) {
        fprintf(stderr, "Failed to load sky: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    sky = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if (!sky) {
        fprintf(stderr, "Failed to convert sky surface\n");
        cleanup_game();
        exit(1);
    }

    // Load the ground image.
    temp = IMG_Load("assets/ground.png");
    if (!temp) {
        fprintf(stderr, "Failed to load ground: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    ground = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if (!ground) {
        fprintf(stderr, "Failed to convert ground surface\n");
        cleanup_game();
        exit(1);
    }

    flashBackground = 0;
    flashStartTime = 0;
    game.running = 1;
    printf("Game initialized successfully!\n");
}

void update_game() {
    // Draw sky covering entire screen.
    SDL_BlitSurface(sky, NULL, game.screen, NULL);

    // Draw ground: position so that its top is at GROUND_LEVEL.
    SDL_Rect groundPos = {0, GROUND_LEVEL, ground->w, ground->h};
    SDL_BlitSurface(ground, NULL, game.screen, &groundPos);

    // Draw game objects, player, and minimap.
    draw_objects();
    draw_player(&player, game.screen);
    draw_minimap();

    // Render on-screen instructions.
    render_text(game.screen, font, "Press S to Save | Press L to Load", 10, SCREEN_HEIGHT - 30);

    SDL_Flip(game.screen);
}

void cleanup_game() {
    printf("Cleaning up resources...\n");
    if (sky)
        SDL_FreeSurface(sky);
    if (ground)
        SDL_FreeSurface(ground);
    if (font)
        TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void save_game() {
    FILE *fp = fopen("save.txt", "w");
    if (!fp) {
        fprintf(stderr, "Failed to open save file for writing\n");
        return;
    }
    // Save player's x, y and the coin's active state.
    fprintf(fp, "%d %d %d\n", player.position.x, player.position.y, coin.active);
    fclose(fp);
    printf("Game saved!\n");
}

void load_game() {
    FILE *fp = fopen("save.txt", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open save file for reading\n");
        return;
    }
    // Use %hd for player's position since they are of type Sint16.
    fscanf(fp, "%hd %hd %d", &player.position.x, &player.position.y, &coin.active);
    fclose(fp);
    printf("Game loaded!\n");
}

