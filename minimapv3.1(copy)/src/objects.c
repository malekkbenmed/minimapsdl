#include "objects.h"
#include "player.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

GameObject platform;
GameObject coin;

int check_collision(SDL_Rect a, SDL_Rect b) {
    // Check if two rectangles overlap
    return !(a.x + a.w < b.x || a.x > b.x + b.w || a.y + a.h < b.y || a.y > b.y + b.h);
}

void init_objects() {
    // Platform initialization
    SDL_Surface* temp = IMG_Load("assets/platform.png");
    if (!temp) {
        fprintf(stderr, "Failed to load platform.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    platform.sprite = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if (!platform.sprite) {
        fprintf(stderr, "Failed to convert platform surface\n");
        cleanup_game();
        exit(1);
    }
    platform.position.x = 300;
    platform.position.y = GROUND_LEVEL - platform.sprite->h; // Raise platform
    platform.position.w = platform.sprite->w;
    platform.position.h = platform.sprite->h;
    platform.active = 1;

    // Coin initialization (unchanged)
    temp = IMG_Load("assets/coin.png");
    if (!temp) {
        fprintf(stderr, "Failed to load coin.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    coin.sprite = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if (!coin.sprite) {
        fprintf(stderr, "Failed to convert coin surface\n");
        cleanup_game();
        exit(1);
    }
    coin.max_frame = 4;
    coin.frame = 0;
    coin.position.x = 600;
    coin.position.y = GROUND_LEVEL - 200;
    coin.position.w = 32;
    coin.position.h = 32;
    coin.active = 1;
}

void update_objects() {
    // Animate the coin if active.
    if (coin.active) {
        static Uint32 last_update = 0;
        const int delay = 100;  // frame delay in milliseconds
        Uint32 now = SDL_GetTicks();
        if (now - last_update > delay) {
            coin.frame = (coin.frame + 1) % coin.max_frame;
            last_update = now;
        }
        // Check collision with the player (global variable 'player' declared in player.c and extern in game.h)
        if (check_collision(player.position, coin.position)) {
            coin.active = 0;
            printf("Coin collected!\n");
            flashBackground = 1;
            flashStartTime = SDL_GetTicks();
        }
    }
}

void draw_objects() {
    // Draw platform.
    if (platform.active && platform.sprite) {
        SDL_BlitSurface(platform.sprite, NULL, game.screen, &platform.position);
    }
    // Draw coin.
    if (coin.active && coin.sprite) {
        int frame_width = coin.sprite->w / coin.max_frame;
        SDL_Rect src = { coin.frame * frame_width, 0, frame_width, coin.sprite->h };
        SDL_BlitSurface(coin.sprite, &src, game.screen, &coin.position);
    }
    // If coin has been collected and flash effect is active, draw a gold glow behind the coin's last location.
    else if (!coin.active && flashBackground) {
        if (SDL_GetTicks() - flashStartTime < 200) {
            SDL_Rect glowRect = coin.position;
            glowRect.x -= 10;
            glowRect.y -= 10;
            glowRect.w += 20;
            glowRect.h += 20;
            SDL_Surface *glow = SDL_CreateRGBSurface(SDL_SWSURFACE, glowRect.w, glowRect.h, 32,
                                                     0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            if (glow) {
                SDL_FillRect(glow, NULL, SDL_MapRGBA(glow->format, 255, 223, 0, 100));
                SDL_SetAlpha(glow, SDL_SRCALPHA, 100);
                SDL_BlitSurface(glow, NULL, game.screen, &glowRect);
                SDL_FreeSurface(glow);
            }
        } else {
            flashBackground = 0;
        }
    }
}

