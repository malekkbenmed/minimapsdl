#include "player.h"
#include "game.h"     // For GROUND_LEVEL, SCREEN_WIDTH, SCREEN_HEIGHT, etc.
#include "objects.h"  // For access to "platform" and check_collision()
#include <stdio.h>
#include <stdlib.h>

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

void init_player(Player* player, SDL_Surface* spriteSheet) {
    player->sprite = spriteSheet;
    // Set the number of frames for each animation
    player->walk_max_frame = 4;  // e.g., 4 walking frames (row 0)
    player->jump_max_frame = 4;  // e.g., 4 jumping frames (row 1)

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

    // Set initial source rectangle (idle uses the first walk frame)
    update_src_rect(player, 0, 0);
}

void handle_input_player(Player* player, const Uint8* keystate) {
    int isMoving = 0;

    // Horizontal movement (handled elsewhere; here we just update the flag and adjust x)
    if (keystate[SDLK_LEFT]) {
        player->position.x -= PLAYER_SPEED;
        isMoving = 1;
    }
    if (keystate[SDLK_RIGHT]) {
        player->position.x += PLAYER_SPEED;
        isMoving = 1;
    }
    player->moving = isMoving;

    // Jump if space is pressed and the player is not already in the air
    if (keystate[SDLK_SPACE] && !player->jumping) {
        player->velocityY = -25;  // Jump force (adjust as needed)
        player->jumping = 1;
    }
}

void update_player(Player* player) {
    // Save previous positions (for collision resolution)
    int prev_x = player->position.x;
    int prev_y = player->position.y;

    // Apply gravity
    player->velocityY += GRAVITY;
    if (player->velocityY > MAX_FALL_SPEED)
        player->velocityY = MAX_FALL_SPEED;
    player->position.y += player->velocityY;

    // --- Collision Resolution with the Platform ---
    if (platform.active && check_collision(player->position, platform.position)) {
        // Calculate the overlap distances along each side
        int overlapLeft   = (player->position.x + player->position.w) - platform.position.x;
        int overlapRight  = (platform.position.x + platform.position.w) - player->position.x;
        int overlapTop    = (player->position.y + player->position.h) - platform.position.y;
        int overlapBottom = (platform.position.y + platform.position.h) - player->position.y;

        // Determine the minimal overlap along each axis
        int xOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
        int yOverlap = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

        // Resolve the collision along the axis with the smallest overlap.
        if (xOverlap < yOverlap) {
            // Collision is predominantly horizontal.
            if (overlapLeft < overlapRight) {
                // Player hit the platform from the left; push player to the left.
                player->position.x = platform.position.x - player->position.w;
            } else {
                // Collision from the right; push player to the right.
                player->position.x = platform.position.x + platform.position.w;
            }
        } else {
            // Collision is predominantly vertical.
            if (overlapTop < overlapBottom) {
                // Player coming from above (landing).
                player->position.y = platform.position.y - player->position.h;
                player->velocityY = 0;
                player->jumping = 0;
            } else {
                // Collision from below; push player down.
                player->position.y = platform.position.y + platform.position.h;
                player->velocityY = 0;
            }
        }
    }

    // Ground collision: ensure the player does not fall below the ground level.
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
        if (now - player->frameTimer > 150) {  // Slowed walking animation (150 ms delay)
            player->frame = (player->frame + 1) % player->walk_max_frame;
            player->frameTimer = now;
        }
        update_src_rect(player, 0, player->frame);  // Use row 0 for walking.
    } else if (player->state == JUMP) {
        if (now - player->frameTimer > 100) {  // Jump animation delay (100 ms)
            player->frame = (player->frame + 1) % player->jump_max_frame;
            player->frameTimer = now;
        }
        update_src_rect(player, 1, player->frame);  // Use row 1 for jumping.
    } else {  // IDLE
        player->frame = 0;
        update_src_rect(player, 0, 0);
    }

    // --- Clamp Horizontal Position ---
    if (player->position.x < 0)
        player->position.x = 0;
    if (player->position.x + player->position.w > SCREEN_WIDTH)
        player->position.x = SCREEN_WIDTH - player->position.w;

    // --- Optional: Safety Clamps for Vertical Position ---
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

