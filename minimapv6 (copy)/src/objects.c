#include "objects.h"
#include "player.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

GameObject platform;
GameObject coin;
GameObject obstacle; 

int check_collision(SDL_Rect a, SDL_Rect b) {
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
    platform.position.y = GROUND_LEVEL - platform.sprite->h; 
    platform.position.w = platform.sprite->w;
    platform.position.h = platform.sprite->h;
    platform.active = 1;

    // Coin initialization
    temp = IMG_Load("assets/coin.png");
    if (!temp) {
        fprintf(stderr, "Failed to load coin.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 0, 0, 0)); // Remove black background
    coin.sprite = SDL_DisplayFormatAlpha(temp);
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

    // Obstacle initialization
    temp = IMG_Load("assets/obstacle.jpg");
    if (!temp) {
        fprintf(stderr, "Failed to load obstacle.jpg: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    obstacle.sprite = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if (!obstacle.sprite) {
        fprintf(stderr, "Failed to convert obstacle surface\n");
        cleanup_game();
        exit(1);
    }

    // Set obstacle size
    obstacle.position.x = 500;  // Positioning obstacle
    obstacle.position.y = GROUND_LEVEL - 150;  // 100px below the coin's Y position
    obstacle.position.w = 50;  // Obstacle width (50px)
    obstacle.position.h = 50;  // Obstacle height (50px)
    obstacle.active = 1;

    // Set movement limits for obstacle (75px to the left and right)
    obstacle.leftLimit = 500 - 75;  // Move 75px to the left of the initial X position
    obstacle.rightLimit = 500 + 170; // Move 75px to the right of the initial X position
    obstacle.velocityX = 2;         // Speed of the obstacle movement
}


void update_objects() {
    // Animate the coin if active
    if (coin.active) {
        static Uint32 last = 0;
        Uint32 now = SDL_GetTicks();
        if (now - last > 100) {
            coin.frame = (coin.frame + 1) % coin.max_frame;
            last = now;
        }
        if (check_collision(player.position, coin.position)) {
            coin.active = 0;
            printf("Coin collected!\n");
            game.score += 1;         
            flashBackground = 1;
            flashStartTime = now;
        }
    }

    // Move the obstacle (left and right movement within defined limits)
    if (obstacle.active) {
        obstacle.position.x += obstacle.velocityX;  // Move horizontally

        // Reverse direction when the obstacle reaches its limits
        if (obstacle.position.x <= obstacle.leftLimit || obstacle.position.x + obstacle.position.w >= obstacle.rightLimit) {
            obstacle.velocityX = -obstacle.velocityX;  // Reverse direction
        }
    }
}




void draw_objects() {
    // 1. Draw platform
    if (platform.active && platform.sprite) {
        SDL_BlitSurface(platform.sprite, NULL, game.screen, &platform.position);
    }

    // 2. Draw coin
    if (coin.active && coin.sprite) {
        int frame_width = coin.sprite->w / coin.max_frame;
        SDL_Rect src = { coin.frame * frame_width, 0, frame_width, coin.sprite->h };

        SDL_Rect dest = coin.position;
        SDL_BlitSurface(coin.sprite, &src, game.screen, &dest);
    }

    // 3. Draw obstacle
    if (obstacle.active && obstacle.sprite) {
        SDL_BlitSurface(obstacle.sprite, NULL, game.screen, &obstacle.position);
    }

    // 4. Draw glow 
    if (!coin.active && flashBackground) {
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


