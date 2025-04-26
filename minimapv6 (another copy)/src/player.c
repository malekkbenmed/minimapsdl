#include "player.h"
#include "game.h"     // For GROUND_LEVEL, SCREEN_WIDTH, SCREEN_HEIGHT, etc.
#include "objects.h"  // For access to platform, coin, and obstacle
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // For memcpy

// Helper function: Update the source rectangle based on the desired row and frame index.
// Assumes the sprite sheet has two rows: row 0 = walking frames, row 1 = jumping frames.
static void update_src_rect(Player* player, int row, int frame_index) {
    int total_frames_per_row = (row == 0) ? player->walk_max_frame : player->jump_max_frame;
    int frame_width = player->sprite->w / total_frames_per_row;
    int frame_height = player->sprite->h / 2; // Two rows: top row is walk, bottom row is jump
    player->srcRect.x = frame_index * frame_width;
    player->srcRect.y = row * frame_height;
    player->srcRect.w = frame_width;
    player->srcRect.h = frame_height;
}

// Helper function: Returns a new SDL_Surface* that is a horizontally flipped version of 'src'.
// The caller is responsible for freeing the returned surface.
static SDL_Surface* flipSurfaceHorizontal(SDL_Surface* src) {
    if (!src) return NULL;

    SDL_Surface* flipped = SDL_CreateRGBSurface(src->flags, src->w, src->h, 
                        src->format->BitsPerPixel, src->format->Rmask,
                        src->format->Gmask, src->format->Bmask, src->format->Amask);
    if (!flipped) return NULL;

    SDL_LockSurface(src);
    SDL_LockSurface(flipped);
    
    int pitch = src->pitch;
    for (int y = 0; y < src->h; ++y) {
        Uint8* srcRow = (Uint8*)src->pixels + y * pitch;
        Uint8* flippedRow = (Uint8*)flipped->pixels + y * pitch;
        for (int x = 0; x < src->w; ++x) {
            memcpy(&flippedRow[(src->w - 1 - x) * src->format->BytesPerPixel],
                   &srcRow[x * src->format->BytesPerPixel],
                   src->format->BytesPerPixel);
        }
    }
    
    SDL_UnlockSurface(src);
    SDL_UnlockSurface(flipped);
    return flipped;
}

void init_player(Player* player, SDL_Surface* spriteSheet) {
    player->sprite = spriteSheet;
    // Generate left-facing sprite by flipping the provided sprite sheet
    player->leftSprite = flipSurfaceHorizontal(spriteSheet);
    if (player->leftSprite) {
        // Reapply the color key so that white (255,255,255) is treated as transparent.
        SDL_SetColorKey(player->leftSprite, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                        SDL_MapRGB(player->leftSprite->format, 255, 255, 255));
    }

    // Set the number of frames for each animation (4 frames for both walking and jumping)
    player->walk_max_frame = 4;
    player->jump_max_frame = 4;

    player->frame = 0;
    player->frameTimer = SDL_GetTicks();

    int frame_width = player->sprite->w / player->walk_max_frame;
    int frame_height = player->sprite->h / 2;  // Two rows: walk and jump
    player->srcRect.w = frame_width;
    player->srcRect.h = frame_height;

    // Initialize player's position (adjust as needed)
    player->position.x = 100;
    player->position.y = GROUND_LEVEL - frame_height - 10;
    player->position.w = frame_width;
    player->position.h = frame_height;

    player->velocityY = 0;
    player->jumping = 0;
    player->moving = 0;
    player->state = IDLE;
    // Default facing direction is right.
    player->facing_right = 1;

    // Set initial source rectangle (idle uses the first walking frame)
    update_src_rect(player, 0, 0);

}

void handle_input_player(Player* player, const Uint8* keystate) {
    int isMoving = 0;

    // Horizontal movement: update flag, adjust x, and record facing direction.
    if (keystate[SDLK_LEFT]) {
        player->position.x -= PLAYER_SPEED;
        isMoving = 1;
        player->facing_right = 0;  // Now facing left
    }
    if (keystate[SDLK_RIGHT]) {
        player->position.x += PLAYER_SPEED;
        isMoving = 1;
        player->facing_right = 1;  // Facing right
    }
    player->moving = isMoving;

    // Jump if the up is pressed and the player is not already airborne.
    if (keystate[SDLK_SPACE] && !player->jumping) {
        player->velocityY = -25;  // Jump force (adjust as needed)
        player->jumping = 1;
    }
}

void update_player(Player* player) {
    // Save previous positions for possible collision handling
    int prev_x = player->position.x;
    int prev_y = player->position.y;

    // Apply gravity (falling effect)
    player->velocityY += GRAVITY;
    if (player->velocityY > MAX_FALL_SPEED)
        player->velocityY = MAX_FALL_SPEED;
    player->position.y += player->velocityY;

    // --- Collision Resolution with the Platform ---
    if (platform.active && check_collision(player->position, platform.position)) {
        int overlapLeft   = (player->position.x + player->position.w) - platform.position.x;
        int overlapRight  = (platform.position.x + platform.position.w) - player->position.x;
        int overlapTop    = (player->position.y + player->position.h) - platform.position.y;
        int overlapBottom = (platform.position.y + platform.position.h) - player->position.y;

        int xOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
        int yOverlap = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

        // Handle horizontal (left-right) collisions
        if (xOverlap < yOverlap) {
            if (overlapLeft < overlapRight) {
                // Player is hitting the right edge of the platform
                player->position.x = platform.position.x - player->position.w; // Stop player at the left side of platform
                // Apply bump effect if moving right
                if (player->moving && player->facing_right) {
                    player->position.x -= 20; // Push player back 20px
                }
            } else {
                // Player is hitting the left edge of the platform
                player->position.x = platform.position.x + platform.position.w; // Stop player at the right side of platform
                // Apply bump effect if moving left
                if (player->moving && !player->facing_right) {
                    player->position.x += 20; // Push player back 20px
                }
            }

            // Decrease health by 5 when colliding with the platform horizontally
            if (game.health > 0) {
                game.health -= 5;
            }
        } else {
            // Handle vertical (top-bottom) collisions with platform
            if (overlapTop < overlapBottom) {
                player->position.y = platform.position.y - player->position.h;
                player->velocityY = 0;
                player->jumping = 0;

                // Decrease health by 5 when colliding with the platform from above
                if (game.health > 0) {
                    game.health -= 5;
                }
            } else {
                player->position.y = platform.position.y + platform.position.h;
                player->velocityY = 0;
            }
        }
    }

    // --- Check if Health is Depleted ---
    if (game.health <= 0) {
        game.health = MAX_HEALTH; // Reset health to 100 after it reaches 0
        load_game();  // Reload game from checkpoint
        return;  // Stop further updates
    }

    // --- Collision Resolution with the Moving Obstacle ---
    if (obstacle.active && check_collision(player->position, obstacle.position)) {
        int overlapLeft   = (player->position.x + player->position.w) - obstacle.position.x;
        int overlapRight  = (obstacle.position.x + obstacle.position.w) - player->position.x;
        int overlapTop    = (player->position.y + player->position.h) - obstacle.position.y;
        int overlapBottom = (obstacle.position.y + obstacle.position.h) - player->position.y;

        int xOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
        int yOverlap = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

        // Handle horizontal (left-right) collisions with the obstacle
        if (xOverlap < yOverlap) {
            if (overlapLeft < overlapRight) {
                // Player is hitting the right edge of the obstacle
                player->position.x = obstacle.position.x - player->position.w; // Stop player at the left side of obstacle
                // Apply bump effect if moving right
                if (player->moving && player->facing_right) {
                    player->position.x -= 20; // Push player back 20px
                }
            } else {
                // Player is hitting the left edge of the obstacle
                player->position.x = obstacle.position.x + obstacle.position.w; // Stop player at the right side of obstacle
                // Apply bump effect if moving left
                if (player->moving && !player->facing_right) {
                    player->position.x += 20; // Push player back 20px
                }
            }
        } else {
            // Handle vertical (top-bottom) collisions with obstacle
            if (overlapTop < overlapBottom) {
                player->position.y = obstacle.position.y - player->position.h;
                player->velocityY = 0;
                player->jumping = 0;
                // Make obstacle carry the player
                player->position.x += obstacle.velocityX;
            } else {
                player->position.y = obstacle.position.y + obstacle.position.h;
                player->velocityY = 0;
            }
        }
    }

    // Ground collision: prevent falling below the ground level.
    if (player->position.y + player->position.h >= GROUND_LEVEL) {
        player->position.y = GROUND_LEVEL - player->position.h;
        player->velocityY = 0;
        player->jumping = 0;
    }

    // --- Determine Animation State ---
    if (player->jumping) {
        player->state = JUMP;
    } else if (player->moving) {
        player->state = WALK;
    } else {
        player->state = IDLE;
    }

    // --- Update Animation Frames ---
    Uint32 now = SDL_GetTicks();
    if (player->state == WALK) {
        if (now - player->frameTimer > 190) {
            player->frame = (player->frame + 1) % player->walk_max_frame;
            player->frameTimer = now;
        }
        update_src_rect(player, 0, player->frame);  // Row 0: walking frames
    } else if (player->state == JUMP) {
        if (now - player->frameTimer > 150) {
            player->frame = (player->frame + 1) % player->jump_max_frame;
            player->frameTimer = now;
        }
        update_src_rect(player, 1, player->frame);  // Row 1: jumping frames
    } else {  // IDLE state
        player->frame = 0;
        update_src_rect(player, 0, 0);
    }

    // --- Clamp Horizontal Position ---
    if (player->position.x < 0)
        player->position.x = 0;
    if (player->position.x + player->position.w > SCREEN_WIDTH)
        player->position.x = SCREEN_WIDTH - player->position.w;

    // --- Safety Clamps for Vertical Position ---
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
    // Choose the appropriate sprite based on facing direction without drawing a white background.
    SDL_Surface* currentSprite = player->facing_right ? player->sprite : player->leftSprite;
    SDL_BlitSurface(currentSprite, &player->srcRect, screen, &player->position);
}

void cleanup_player(Player* player) {
    if (player->sprite) SDL_FreeSurface(player->sprite);
    if (player->leftSprite) SDL_FreeSurface(player->leftSprite);
}
