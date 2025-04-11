#ifndef PLAYER_H
#define PLAYER_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Constants for player movement.
#define PLAYER_SPEED 5
#define GRAVITY 1

typedef struct Player {
    SDL_Surface* sprite;  // The sprite sheet for the player.
    SDL_Rect position;    // Where the player is on the screen.
    SDL_Rect srcRect;     // The portion of the sprite sheet to display.
    int frame;            // Current frame of animation.
    int max_frame;        // Total frames in the walking animation (should be 4).
    Uint32 frameTimer;    // Timer to control the animation speed.
    int velocityY;        // Vertical velocity for gravity and jump.
    int jumping;          // 1 if the player is in the air, 0 if on the ground.
} Player;

void init_player(Player* player, SDL_Surface* spriteSheet);
void handle_input_player(Player* player, const Uint8* keystate);
void update_player(Player* player);
void draw_player(Player* player, SDL_Surface* screen);

#endif
