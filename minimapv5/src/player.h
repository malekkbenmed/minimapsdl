#ifndef PLAYER_H
#define PLAYER_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Movement and physics constants
#define PLAYER_SPEED 5
#define GRAVITY 1
#define MAX_FALL_SPEED 25  // Maximum downward speed

// Define player animation states
typedef enum {
    IDLE,
    WALK,
    JUMP
} PlayerState;

typedef struct Player {
    SDL_Surface* sprite;        // Normal sprite
    SDL_Surface* leftSprite;    // Left-facing sprite
    SDL_Rect srcRect;           // Rectangle for the animation frame
    SDL_Rect position;          // Position of the player

    int velocityX;              // Horizontal velocity
    int velocityY;              // Vertical velocity
    int state;                  // State of the player (idle, walking, jumping)
    int jumping;                // Jumping flag
    int moving;                 // Moving flag
    int facing_right;           // True if facing right, false if facing left

    int frame;                  // Frame index for animation
    Uint32 frameTimer;          // Timer for switching frames

    int walk_max_frame;         // Maximum frames for walking animation
    int jump_max_frame;         // Maximum frames for jumping animation

} Player;

void init_player(Player* player, SDL_Surface* spriteSheet);
void handle_input_player(Player* player, const Uint8* keystate);
void update_player(Player* player);
void draw_player(Player* player, SDL_Surface* screen);
void cleanup_player(Player* player);

#endif

