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
    // Platform initialization (solid obstacle)
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

    // Coin initialization (collectible object)
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
    // When the coin is collected, disable it and trigger the flash effect.
    if(coin.active && check_collision(player.position, coin.position)) {
        coin.active = 0;
        printf("Coin collected!\n");
        flashBackground = 1;
        flashStartTime = SDL_GetTicks();
    }
}

void draw_objects() {
    if (coin.active) {
        // Flash effect behind coin (light golden glow)
        if (flashBackground && SDL_GetTicks() - flashStartTime < 200) {
            SDL_Rect glowRect = coin.position;

            // Inflate the rectangle around the coin for a glow effect
            glowRect.x -= 10;
            glowRect.y -= 10;
            glowRect.w += 20;
            glowRect.h += 20;

            // Create a semi-transparent surface for the glow
            SDL_Surface *glowSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, glowRect.w, glowRect.h, 32,
                                                             0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_FillRect(glowSurface, NULL, SDL_MapRGBA(glowSurface->format, 255, 223, 0, 100)); // Gold with alpha
            SDL_SetAlpha(glowSurface, SDL_SRCALPHA, 100);  // Light transparency

            SDL_BlitSurface(glowSurface, NULL, game.screen, &glowRect);
            SDL_FreeSurface(glowSurface);
        }

        // Draw the coin itself
        SDL_BlitSurface(coin.sprite, NULL, game.screen, &coin.position);
    }

    if (platform.active) {
        SDL_BlitSurface(platform.sprite, NULL, game.screen, &platform.position);
    }
}


