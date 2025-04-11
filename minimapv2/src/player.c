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
    // Place the player at the center and on the ground.
    player.position.x = SCREEN_WIDTH / 2 - 32;
    player.position.y = GROUND_LEVEL - 64;
    player.position.w = 64;
    player.position.h = 64;
    player.velocity_y = 0;
    player.grounded = 1;
}

void update_player() {
    SDL_Rect prevPos = player.position;
    const Uint8 *keys = SDL_GetKeyState(NULL);

    // Horizontal movement.
    if (keys[SDLK_LEFT])
        player.position.x -= 5;
    if (keys[SDLK_RIGHT])
        player.position.x += 5;

    // Horizontal collision: If colliding with platform and not exactly standing on top.
    if (platform.active && check_collision(player.position, platform.position)) {
        if ((player.position.y + player.position.h) != platform.position.y)
            player.position.x = prevPos.x;
    }

    // Vertical movement.
    prevPos.y = player.position.y;
    if (keys[SDLK_SPACE] && player.grounded) {
        player.velocity_y = JUMP_FORCE;
        player.grounded = 0;
    }
    player.velocity_y += GRAVITY;
    player.position.y += player.velocity_y;

    // Vertical collision resolution with platform.
    if (platform.active && check_collision(player.position, platform.position)) {
        if (prevPos.y + player.position.h <= platform.position.y + 5) {
            player.position.y = platform.position.y - player.position.h;
            player.grounded = 1;
        } else if (prevPos.y >= platform.position.y + platform.position.h - 5) {
            player.position.y = platform.position.y + platform.position.h;
        } else {
            player.position.y = prevPos.y;
        }
        player.velocity_y = 0;
    }

    // Ground collision.
    if (player.position.y >= GROUND_LEVEL) {
        player.position.y = GROUND_LEVEL;
        player.velocity_y = 0;
        player.grounded = 1;
    }

    // Clamp player horizontally.
    if (player.position.x < 0)
        player.position.x = 0;
    if (player.position.x > SCREEN_WIDTH - player.position.w)
        player.position.x = SCREEN_WIDTH - player.position.w;
}

void draw_player() {
    SDL_BlitSurface(player.sprite, NULL, game.screen, &player.position);
}

