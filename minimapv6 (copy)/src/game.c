#include "game.h"
#include "objects.h"
#include "minimap.h"
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>

GameState    game;
int          flashBackground = 0;
Uint32       flashStartTime  = 0;
TTF_Font*    font            = NULL;
SDL_Surface *sky = NULL, *city = NULL, *ground = NULL;
Player       player;
int*        groundHeight = NULL;

Uint32 get_pixel(SDL_Surface* surface, int x, int y) {
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return 0;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
    return *(Uint32*)p;
}

void generate_ground_heightmap(SDL_Surface* ground) {
    if (groundHeight) free(groundHeight);
    groundHeight = malloc(SCREEN_WIDTH * sizeof(int));
    Uint32 transparent = SDL_MapRGB(ground->format, 0, 0, 0);

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        groundHeight[x] = GROUND_LEVEL + ground->h;
        if (x >= ground->w) continue;

        for (int y = 0; y < ground->h; y++) {
            Uint32 pixel = get_pixel(ground, x, y);
            if (pixel != transparent) {
                groundHeight[x] = GROUND_LEVEL + y;
                break;
            }
        }
    }
}

void render_text(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* ts = TTF_RenderText_Blended(font, text, white);
    if (ts) {
        SDL_Rect dst = {x, y, ts->w, ts->h};
        SDL_BlitSurface(ts, NULL, screen, &dst);
        SDL_FreeSurface(ts);
    }
}

void init_game() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr,"SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }
    if (!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) {
        fprintf(stderr,"IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }
    if (TTF_Init()==-1) {
        fprintf(stderr,"TTF_Init: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        exit(1);
    }

    font = TTF_OpenFont("assets/arial.ttf", 16);
    if (!font) {
        fprintf(stderr,"TTF_OpenFont: %s\n", TTF_GetError());
        cleanup_game();
        exit(1);
    }

    game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!game.screen) {
        fprintf(stderr,"SDL_SetVideoMode: %s\n", SDL_GetError());
        cleanup_game();
        exit(1);
    }

    SDL_Surface* tmp = IMG_Load("assets/sky.jpg");
    if (!tmp){ fprintf(stderr,"sky.jpg: %s\n", IMG_GetError()); cleanup_game(); exit(1); }
    sky = SDL_DisplayFormat(tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("assets/city.png");
    if (!tmp){ fprintf(stderr,"city.png: %s\n", IMG_GetError()); cleanup_game(); exit(1); }
    city = SDL_DisplayFormat(tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("assets/ground.png");
    if (!tmp){ fprintf(stderr,"ground.png: %s\n", IMG_GetError()); cleanup_game(); exit(1); }
    ground = SDL_DisplayFormat(tmp);
    SDL_FreeSurface(tmp);

    generate_ground_heightmap(ground);

    game.running = 1;
    game.health  = MAX_HEALTH;
    game.score   = 0;
    save_game();
}

void update_game() {
    SDL_BlitSurface(sky,  NULL, game.screen, NULL);
    SDL_BlitSurface(city, NULL, game.screen, NULL);
    SDL_Rect gpos = {0, GROUND_LEVEL, ground->w, ground->h};
    SDL_BlitSurface(ground, NULL, game.screen, &gpos);

    draw_objects();
    draw_player(&player, game.screen);
    draw_minimap();

    render_text(game.screen, font, "Press S to Save | Press L to Load", 10, SCREEN_HEIGHT - 30);

    SDL_Rect hb_bg = {10,10,200,20};
    SDL_FillRect(game.screen, &hb_bg, SDL_MapRGB(game.screen->format,100,100,100));
    int hw = (int)((game.health/(float)MAX_HEALTH) * hb_bg.w);
    SDL_Rect hb_fg = {10,10,hw,20};
    SDL_FillRect(game.screen, &hb_fg, SDL_MapRGB(game.screen->format,200,0,0));

    char buf[32];
    snprintf(buf, sizeof(buf), "Score: %d", game.score);
    render_text(game.screen, font, buf, 10, 40);

    SDL_Flip(game.screen);
}

void save_game() {
    FILE* f = fopen("save.txt","w");
    if (!f) {
        fprintf(stderr,"Cannot write save.txt\n");
        return;
    }
    fprintf(f,"%d %d %d %d %d\n",
            player.position.x,
            player.position.y,
            coin.active,
            game.health,
            game.score);
    fclose(f);
    printf("Game saved successfully!\n");
}

void load_game() {
    FILE* f = fopen("save.txt","r");
    if (!f) {
        fprintf(stderr,"No save file\n");
        return;
    }
    fscanf(f,"%hd %hd %d %d %d",
           &player.position.x,
           &player.position.y,
           &coin.active,
           &game.health,
           &game.score);
    fclose(f);
    if (game.health <= 0) {
        game.health = MAX_HEALTH;
    }
    printf("Game loaded successfully!\n");
}

void cleanup_game() {
    if (sky) SDL_FreeSurface(sky);
    if (city) SDL_FreeSurface(city);
    if (ground) SDL_FreeSurface(ground);
    if (font) TTF_CloseFont(font);
    free(groundHeight);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
