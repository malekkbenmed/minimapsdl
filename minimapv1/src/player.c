#include "player.h"
#include "game.h"
#include "objects.h"

Player player;

void init_player() {
    printf("Loading player sprite...\n");
    SDL_Surface *temp = IMG_Load("assets/player.png");
    if(!temp) {
        fprintf(stderr, "Failed to load player.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    player.sprite = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if(!player.sprite) {
        fprintf(stderr, "Failed to convert player surface\n");
        cleanup_game();
        exit(1);
    }
    // Position the player at the horizontal center and on the ground.
    player.position.x = SCREEN_WIDTH / 2 - 32;
    player.position.y = GROUND_LEVEL - 64;
    player.position.w = 64;
    player.position.h = 64;
    player.velocity_y = 0;
    player.grounded = 1;  // the player starts on the ground.
}

void update_player() {
    // Save the previous position for collision resolution.
    SDL_Rect prevPos = player.position;
    
    const Uint8 *keys = SDL_GetKeyState(NULL);

    // -------- Horizontal Movement --------
    if (keys[SDLK_LEFT])
        player.position.x -= 5;
    if (keys[SDLK_RIGHT])
        player.position.x += 5;

    // Horizontal collision resolution with platform:
    if (platform.active && check_collision(player.position, platform.position)) {
        /* 
         * Allow horizontal movement if the collision is solely a top-contact.
         * That means if the player's bottom edge is exactly at the platform's top.
         * (This check lets the player move while on top of the platform.)
         */
        if ((player.position.y + player.position.h) != platform.position.y) {
            // If not exactly on top, revert to previous X position.
            player.position.x = prevPos.x;
        }
    }

    // -------- Vertical Movement --------
    // Update the previous vertical position before processing vertical movement.
    prevPos.y = player.position.y;

    // Jump input (only if on ground)
    if (keys[SDLK_SPACE] && player.grounded) {
        player.velocity_y = JUMP_FORCE;
        player.grounded = 0;
    }
    
    // Apply gravity and update vertical position.
    player.velocity_y += GRAVITY;
    player.position.y += player.velocity_y;
    
    // Vertical collision resolution with platform.
    if (platform.active && check_collision(player.position, platform.position)) {
        // Determine collision direction based on previous vertical position.
        // If player was above the platform, snap them to the top.
        if (prevPos.y + player.position.h <= platform.position.y + 5) {
            player.position.y = platform.position.y - player.position.h;
            player.grounded = 1;
        }
        // Otherwise, if the player was below the platform, push them down.
        else if (prevPos.y >= platform.position.y + platform.position.h - 5) {
            player.position.y = platform.position.y + platform.position.h;
        }
        else {
            // Otherwise, revert vertical movement.
            player.position.y = prevPos.y;
        }
        // In any case, stop vertical velocity.
        player.velocity_y = 0;
    }
    
    // Collision with ground.
    if (player.position.y >= GROUND_LEVEL) {
        player.position.y = GROUND_LEVEL;
        player.velocity_y = 0;
        player.grounded = 1;
    }
    
    // Clamp horizontal boundaries.
    if (player.position.x < 0)
        player.position.x = 0;
    if (player.position.x > SCREEN_WIDTH - player.position.w)
        player.position.x = SCREEN_WIDTH - player.position.w;
}

void draw_player() {
    SDL_BlitSurface(player.sprite, NULL, game.screen, &player.position);
}

