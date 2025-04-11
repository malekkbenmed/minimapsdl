#include "objects.h"
#include "player.h"

GameObject platform;
GameObject coin;

int check_collision(SDL_Rect a, SDL_Rect b) {
    return !(a.x + a.w < b.x || 
             a.x > b.x + b.w ||
             a.y + a.h < b.y || 
             a.y > b.y + b.h);
}

void init_objects() {
    // Initialize platform (solid object on the left)
    printf("Initializing platform...\n");
    SDL_Surface *temp = IMG_Load("assets/platform.png");
    if(!temp) {
        fprintf(stderr, "Failed to load platform.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    platform.sprite = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!platform.sprite) {
        fprintf(stderr, "Failed to convert platform surface\n");
        cleanup_game();
        exit(1);
    }
    platform.position.x = 100;
    platform.position.y = GROUND_LEVEL - 128;
    platform.position.w = 128;
    platform.position.h = 32;
    platform.active = 1;

    // Initialize coin (collectible object on the right)
    printf("Initializing coin...\n");
    temp = IMG_Load("assets/coin.png");
    if(!temp) {
        fprintf(stderr, "Failed to load coin.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    coin.sprite = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!coin.sprite) {
        fprintf(stderr, "Failed to convert coin surface\n");
        cleanup_game();
        exit(1);
    }
    coin.position.x = 600;
    coin.position.y = GROUND_LEVEL - 200;
    coin.position.w = 32;
    coin.position.h = 32;
    coin.active = 1;
}

void update_objects() {
    // If the coin is active and the player collides with it, disable the coin.
    if(coin.active && check_collision(player.position, coin.position)) {
        coin.active = 0;
        printf("Coin collected!\n");
    }
}

void draw_objects() {
    if(platform.active && platform.sprite) {
        SDL_BlitSurface(platform.sprite, NULL, game.screen, &platform.position);
    }
    if(coin.active && coin.sprite) {
        SDL_BlitSurface(coin.sprite, NULL, game.screen, &coin.position);
    }
}

