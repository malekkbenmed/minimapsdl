#include "game.h"
#include "objects.h"
#include "minimap.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

GameState    game;
int          flashBackground = 0;
Uint32       flashStartTime  = 0;
TTF_Font*    font            = NULL;
SDL_Surface *sky = NULL, *city = NULL, *ground = NULL;
Player       player;

void render_text(SDL_Surface* screen,
                 TTF_Font*      font,
                 const char*    text,
                 int            x,
                 int            y)
{
    SDL_Color white = {255,255,255,255};
    SDL_Surface* ts = TTF_RenderText_Blended(font, text, white);
    if (ts) {
        SDL_Rect dst = {x,y,ts->w,ts->h};
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

    // Load backgrounds
    {
        SDL_Surface* tmp = IMG_Load("assets/sky.jpg");
        if (!tmp){ fprintf(stderr,"sky.jpg: %s\n", IMG_GetError()); cleanup_game(); exit(1); }
        sky = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);
    }
    {
        SDL_Surface* tmp = IMG_Load("assets/city.jpg");
        if (!tmp){ fprintf(stderr,"city.jpg: %s\n", IMG_GetError()); cleanup_game(); exit(1); }
        city = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);
    }
    {
        SDL_Surface* tmp = IMG_Load("assets/ground.jpg");
        if (!tmp){ fprintf(stderr,"ground.jpg: %s\n", IMG_GetError()); cleanup_game(); exit(1); }
        ground = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);
    }

    // Game state
    game.running = 1;
    game.health  = MAX_HEALTH;
    game.score   = 0;

    // initial checkpoint
    save_game();
}

void update_game() {
    // background layers
    SDL_BlitSurface(sky,  NULL, game.screen, NULL);
    SDL_BlitSurface(city, NULL, game.screen, NULL);
    SDL_Rect gpos = {0, GROUND_LEVEL, ground->w, ground->h};
    SDL_BlitSurface(ground, NULL, game.screen, &gpos);

    // world
    draw_objects();
    draw_player(&player, game.screen);
    draw_minimap();

    // instructions
    render_text(game.screen, font, "Press S to Save | Press L to Load",
                10, SCREEN_HEIGHT - 30);

    // Health bar
    SDL_Rect hb_bg = {10,10,200,20};
    SDL_FillRect(game.screen, &hb_bg,
                 SDL_MapRGB(game.screen->format,100,100,100));
    int hw = (int)((game.health/(float)MAX_HEALTH) * hb_bg.w);
    SDL_Rect hb_fg = {10,10,hw,20};
    SDL_FillRect(game.screen, &hb_fg,
                 SDL_MapRGB(game.screen->format,200,0,0));

    // Score text
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
    // x y coinState health score
    fprintf(f,"%d %d %d %d %d\n",
            player.position.x,
            player.position.y,
            coin.active,
            game.health,
            game.score);
    fclose(f);
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

    // Reset health to 100 if it was less than that when loading the checkpoint
    if (game.health <= 0) {
        game.health = MAX_HEALTH;
    }
}


void cleanup_game() {
    if (sky)    SDL_FreeSurface(sky);
    if (city)   SDL_FreeSurface(city);
    if (ground) SDL_FreeSurface(ground);
    if (font)   TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

