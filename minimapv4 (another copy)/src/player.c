#include "player.h"
#include "game.h"     // For GROUND_LEVEL, SCREEN_WIDTH, SCREEN_HEIGHT, etc.
#include "objects.h"  // For access to platform and check_collision()
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

    // Jump if the space key is pressed and the player is not already airborne.
    if (keystate[SDLK_SPACE] && !player->jumping) {
        player->velocityY = -25;  // Jump force (adjust as needed)
        player->jumping = 1;
    }
}

// Adjust update_player to accept deltaTime in milliseconds.
void update_player(Player* player, Uint32 deltaTime) {
    // Instead of directly adding GRAVITY once per frame,
    // scale gravity by deltaTime. Assume the base frame time is 16ms.
    float timeFactor = deltaTime / 16.0f;

    // Apply gravity (scaled by deltaTime)
    player->velocityY += GRAVITY * timeFactor;
    if (player->velocityY > MAX_FALL_SPEED)
        player->velocityY = MAX_FALL_SPEED;
    player->position.y += player->velocityY * timeFactor;

    // (Rest of collision resolution remains unchanged.)

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
    // Use deltaTime to decide whether to change frame instead of fixed values.
    if (player->state == WALK) {
        if (now - player->frameTimer > 190) {
            player->frame = (player->frame + 1) % player->walk_max_frame;
            player->frameTimer = now;
        }
        update_src_rect(player, 0, player->frame);
    } else if (player->state == JUMP) {
        if (now - player->frameTimer > 150) {
            player->frame = (player->frame + 1) % player->jump_max_frame;
            player->frameTimer = now;
        }
        update_src_rect(player, 1, player->frame);
    } else {
        player->frame = 0;
        update_src_rect(player, 0, 0);
    }

    // --- Clamp Position ---
    if (player->position.x < 0)
        player->position.x = 0;
    if (player->position.x + player->position.w > SCREEN_WIDTH)
        player->position.x = SCREEN_WIDTH - player->position.w;
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

