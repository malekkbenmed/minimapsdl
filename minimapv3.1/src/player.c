#include "player.h"
#include "game.h"
#include "objects.h" // For access to the "platform" obstacle and check_collision()
#include <stdio.h>
#include <stdlib.h>

// Helper function: Resolves collision between the player's rectangle and a block (platform)
// by computing the minimal overlap on each axis.
static void resolve_collision(SDL_Rect *playerRect, int *velocityY, SDL_Rect block) {
    // Calculate overlaps in each direction.
    int overlapLeft   = (playerRect->x + playerRect->w) - block.x;
    int overlapRight  = (block.x + block.w) - playerRect->x;
    int overlapTop    = (playerRect->y + playerRect->h) - block.y;
    int overlapBottom = (block.y + block.h) - playerRect->y;
    
    // Find the minimum overlap on each axis.
    int xOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
    int yOverlap = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;
    
    // Resolve collision along the axis with the smaller overlap.
    if (xOverlap < yOverlap) {
        // Horizontal collision resolution.
        if (overlapLeft < overlapRight) {
            // Collision from left: push player to the left of the block.
            playerRect->x = block.x - playerRect->w;
        } else {
            // Collision from right: push player to the right of the block.
            playerRect->x = block.x + block.w;
        }
    } else {
        // Vertical collision resolution.
        if (overlapTop < overlapBottom) {
            // Player falling onto the block: place player on top.
            playerRect->y = block.y - playerRect->h;
            *velocityY = 0;
        } else {
            // Player hitting the block from below: push player down.
            playerRect->y = block.y + block.h;
            *velocityY = 0;
        }
    }
}

void init_player(Player* player, SDL_Surface* spriteSheet) {
    player->sprite = spriteSheet;
    player->max_frame = 4;  // Adjust according to your sprite sheet.
    player->frame = 0;
    player->frameTimer = SDL_GetTicks();
    player->srcRect.w = player->sprite->w / player->max_frame;
    player->srcRect.h = player->sprite->h;
    
    // Initialize player's starting position and size.
    player->position.x = 100;
    player->position.y = GROUND_LEVEL - player->srcRect.h - 10;
    player->position.w = player->srcRect.w;
    player->position.h = player->srcRect.h;
    
    player->velocityY = 0;
    player->jumping = 0;
}

void handle_input_player(Player* player, const Uint8* keystate) {
    // Handle horizontal movement.
    if (keystate[SDLK_LEFT])  {
        player->position.x -= PLAYER_SPEED;
    }
    if (keystate[SDLK_RIGHT]) {
        player->position.x += PLAYER_SPEED;
    }
    
    // Allow jumping if not already in the air.
    // You might add additional checks if you want one-way platform behavior.
    if (keystate[SDLK_SPACE] && !player->jumping) {
        player->velocityY = -22;  // Jump force, adjust as needed.
        player->jumping = 1;
    }
}

void update_player(Player* player) {
    // Save previous position (could be used for debugging or more complex resolution).
    int prev_x = player->position.x;
    int prev_y = player->position.y;
    
    // --- Update Animation ---
    Uint32 now = SDL_GetTicks();
    if (now - player->frameTimer >= 100) {  // Change frame every 100 ms (adjust as necessary)
        player->frame = (player->frame + 1) % player->max_frame;
        player->srcRect.x = player->frame * player->srcRect.w;
        player->frameTimer = now;
    }
    
    // --- Apply Gravity ---
    player->velocityY += GRAVITY;
    if (player->velocityY > MAX_FALL_SPEED) {
        player->velocityY = MAX_FALL_SPEED;
    }
    player->position.y += player->velocityY;
    
    // --- Collision with Platform (Obstacle) ---
    // 'platform' is assumed to be defined globally in objects.c and declared in objects.h.
    if (platform.active && check_collision(player->position, platform.position)) {
        resolve_collision(&player->position, &player->velocityY, platform.position);
        // When resolved vertically, if the player lands exactly on top of the platform,
        // mark jumping as false.
        if (player->position.y + player->position.h == platform.position.y) {
            player->jumping = 0;
        }
    }
    
    // --- Ground Collision ---
    if (player->position.y + player->position.h >= GROUND_LEVEL) {
        player->position.y = GROUND_LEVEL - player->position.h;
        player->velocityY = 0;
        player->jumping = 0;
    }
    
    // --- Horizontal Screen Bounds ---
    if (player->position.x < 0) {
        player->position.x = 0;
    }
    if (player->position.x + player->position.w > SCREEN_WIDTH) {
        player->position.x = SCREEN_WIDTH - player->position.w;
    }
    
    // --- Optional Vertical Clamps (safety net) ---
    if (player->position.y < 0) {
        player->position.y = 0;
        player->velocityY = 0;
    }
    if (player->position.y + player->position.h > SCREEN_HEIGHT) {
        player->position.y = SCREEN_HEIGHT - player->position.h;
        player->velocityY = 0;
    }
}

void draw_player(Player* player, SDL_Surface* screen) {
    SDL_BlitSurface(player->sprite, &player->srcRect, screen, &player->position);
}

