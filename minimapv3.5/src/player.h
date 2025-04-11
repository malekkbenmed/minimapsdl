#ifndef PLAYER_H
#define PLAYER_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Movement and physics constants
#define PLAYER_SPEED 5
#define GRAVITY 1
#define MAX_FALL_SPEED 20  // Maximum downward speed

// Define player animation states
typedef enum {
    IDLE,
    WALK,
    JUMP
} PlayerState;

typedef struct Player {
    SDL_Surface* sprite;     // Sprite sheet (two rows: row 0 for walk, row 1 for jump)
    SDL_Rect position;       // On-screen destination rectangle
    SDL_Rect srcRect;        // Source rectangle (which frame to draw)
    int frame;               // Frame index for current animation
    int walk_max_frame;      // Number of frames in the walk row
    int jump_max_frame;      // Number of frames in the jump row
    Uint32 frameTimer;       // Used to time animation changes
    int velocityY;           // Vertical velocity (gravity and jump)
    int jumping;             // 0 if on the ground, 1 if airborne
    int moving;              // 0 if idle, 1 if moving horizontally
    PlayerState state;       // Current state (IDLE, WALK, or JUMP)
} Player;

void init_player(Player* player, SDL_Surface* spriteSheet);
void handle_input_player(Player* player, const Uint8* keystate);
void update_player(Player* player);
void draw_player(Player* player, SDL_Surface* screen);

#endif

