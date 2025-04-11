#ifndef PLAYER_H
#define PLAYER_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Movement and physics constants
#define PLAYER_SPEED 5
#define GRAVITY 1
#define MAX_FALL_SPEED 20  // Maximum downward speed

typedef struct Player {
    SDL_Surface* sprite;     // Player sprite sheet
    SDL_Rect position;       // Destination rectangle (position & size)
    SDL_Rect srcRect;        // Source rectangle for animation
    int frame;               // Current animation frame
    int max_frame;           // Total number of frames in sprite sheet
    Uint32 frameTimer;       // Timer used for animation delays
    int velocityY;           // Vertical velocity (for gravity and jumping)
    int jumping;             // 0 if not jumping/airborne, 1 if currently jumping
} Player;

// Initializes the player with the given sprite sheet.
void init_player(Player* player, SDL_Surface* spriteSheet);

// Processes keyboard input to move the player.
void handle_input_player(Player* player, const Uint8* keystate);

// Updates the player's position, applies gravity, and resolves collisions.
void update_player(Player* player);

// Renders the player on the given screen.
void draw_player(Player* player, SDL_Surface* screen);

#endif

