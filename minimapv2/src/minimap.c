#include "minimap.h"
#include "game.h"
#include "player.h"
#include "objects.h"

SDL_Surface *minimap_bg = NULL;
SDL_Surface *player_icon = NULL;
SDL_Surface *platform_icon = NULL;
SDL_Surface *coin_icon = NULL;

SDL_Rect minimap_rect = {800 - 200, 10, 250, 150};
const float SCALE = 0.2f;

void init_minimap() {
    printf("Initializing minimap...\n");
    SDL_Surface *temp = IMG_Load("assets/minimap.png");
    if(!temp) {
        fprintf(stderr, "Failed to load minimap.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    minimap_bg = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!minimap_bg) {
        fprintf(stderr, "Failed to convert minimap background\n");
        cleanup_game();
        exit(1);
    }

    temp = IMG_Load("assets/minimap_player.png");
    if(!temp) {
        fprintf(stderr, "Failed to load minimap_player.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    player_icon = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!player_icon) {
        fprintf(stderr, "Failed to convert player icon\n");
        cleanup_game();
        exit(1);
    }

    temp = IMG_Load("assets/minimap_platform.png");
    if(!temp) {
        fprintf(stderr, "Failed to load minimap_platform.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    platform_icon = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!platform_icon) {
        fprintf(stderr, "Failed to convert platform icon: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }

    temp = IMG_Load("assets/minimap_coin.png");
    if(!temp) {
        fprintf(stderr, "Failed to load minimap_coin.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    coin_icon = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!coin_icon) {
        fprintf(stderr, "Failed to convert coin icon: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
}

void draw_minimap() {
    SDL_BlitSurface(minimap_bg, NULL, game.screen, &minimap_rect);

    // Draw player icon on the minimap.
    SDL_Rect p_pos = {
        minimap_rect.x + (int)(player.position.x * SCALE),
        minimap_rect.y + (int)(player.position.y * SCALE),
        (int)(player.position.w * SCALE),
        (int)(player.position.h * SCALE)
    };
    SDL_BlitSurface(player_icon, NULL, game.screen, &p_pos);

    // Draw platform icon on the minimap.
    if(platform.active && platform_icon) {
        SDL_Rect plat_pos = {
            minimap_rect.x + (int)(platform.position.x * SCALE),
            minimap_rect.y + (int)(platform.position.y * SCALE),
            (int)(platform.position.w * SCALE),
            (int)(platform.position.h * SCALE)
        };
        SDL_BlitSurface(platform_icon, NULL, game.screen, &plat_pos);
    }

    // Draw coin icon on the minimap.
    if(coin.active && coin_icon) {
        SDL_Rect coin_pos = {
            minimap_rect.x + (int)(coin.position.x * SCALE),
            minimap_rect.y + (int)(coin.position.y * SCALE),
            (int)(coin.position.w * SCALE),
            (int)(coin.position.h * SCALE)
        };
        SDL_BlitSurface(coin_icon, NULL, game.screen, &coin_pos);
    }
}

