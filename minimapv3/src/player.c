#include "player.h"
#include "game.h"  // For GROUND_LEVEL
#include <SDL/SDL_image.h>
#include <stdio.h>

void init_player(Player* player, SDL_Surface* spriteSheet) {
    // Use provided sprite sheet (should be already converted with SDL_DisplayFormat)
    player->sprite = spriteSheet;
    player->max_frame = 4;
    player->frame = 0;
    player->frameTimer = SDL_GetTicks();
    
    // Assume the sprite sheet has 1 row with 4 frames.
    int frameWidth = player->sprite->w / player->max_frame;
    int frameHeight = player->sprite->h;
    
    // Set the source rectangle for animation (initial frame).
    player->srcRect.x = 0;
    player->srcRect.y = 0;
    player->srcRect.w = frameWidth;
    player->srcRect.h = frameHeight;
    
    // Set initial position: horizontal position 100; vertical such that the bottom is 10 pixels above ground.
    player->position.x = 100;
    player->position.y = GROUND_LEVEL - frameHeight - 10;
    player->position.w = frameWidth;
    player->position.h = frameHeight;
    
    // Initialize vertical velocity and jumping state.
    player->velocityY = 0;
    player->jumping = 0;
}

void handle_input_player(Player* player, const Uint8* keystate) {
    // Horizontal movement.
    if (keystate[SDLK_LEFT]) {
        player->position.x -= PLAYER_SPEED;
    }
    if (keystate[SDLK_RIGHT]) {
        player->position.x += PLAYER_SPEED;
    }
    // Jump if space is pressed and not already jumping.
    if (keystate[SDLK_SPACE] && !player->jumping) {
        player->velocityY = -15;  // Jump strength.
        player->jumping = 1;
    }
}

void update_player(Player* player) {
    Uint32 now = SDL_GetTicks();
    // Update the animation frame every 100 ms.
    if (now - player->frameTimer >= 100) {
        player->frame = (player->frame + 1) % player->max_frame;
        player->srcRect.x = player->frame * player->srcRect.w;
        player->frameTimer = now;
    }
    
    // Apply gravity.
    player->velocityY += GRAVITY;
    player->position.y += player->velocityY;
    
    // Ground collision.
    if (player->position.y + player->position.h >= GROUND_LEVEL) {
        player->position.y = GROUND_LEVEL - player->position.h;
        player->velocityY = 0;
        player->jumping = 0;
    }
}

void draw_player(Player* player, SDL_Surface* screen) {
    SDL_BlitSurface(player->sprite, &player->srcRect, screen, &player->position);
}

