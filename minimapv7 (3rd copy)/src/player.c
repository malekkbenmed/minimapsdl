#include "player.h"
#include "game.h"
#include "objects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void update_src_rect(Player* player, int row, int frame_index) {
    int total_frames_per_row = (row == 0) ? player->walk_max_frame : player->jump_max_frame;
    if (total_frames_per_row <= 0) total_frames_per_row = 1;
    
    int frame_width = player->sprite->w / total_frames_per_row;
    int frame_height = player->sprite->h / 2;
    
    frame_index = frame_index % total_frames_per_row;
    
    player->srcRect.x = frame_index * frame_width;
    player->srcRect.y = row * frame_height;
    player->srcRect.w = frame_width;
    player->srcRect.h = frame_height;
}

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
    player->leftSprite = flipSurfaceHorizontal(spriteSheet);
    if (player->leftSprite) {
        SDL_SetColorKey(player->leftSprite, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                        SDL_MapRGB(player->leftSprite->format, 255, 255, 255));
    }

    player->walk_max_frame = 4;
    player->jump_max_frame = 4;
    player->frame = 0;
    player->frameTimer = SDL_GetTicks();

    int frame_width = player->sprite->w / player->walk_max_frame;
    int frame_height = player->sprite->h / 2;
    player->srcRect.w = frame_width;
    player->srcRect.h = frame_height;

    player->position.x = 100;
    player->position.y = GROUND_LEVEL - frame_height - 10;
    player->position.w = frame_width;
    player->position.h = frame_height;

    player->velocityY = 0;
    player->jumping = 0;
    player->moving = 0;
    player->state = IDLE;
    player->facing_right = 1;
    update_src_rect(player, 0, 0);
}

void handle_input_player(Player* player, const Uint8* keystate) {
    int isMoving = 0;
    if (keystate[SDLK_LEFT]) {
        player->position.x -= PLAYER_SPEED;
        isMoving = 1;
        player->facing_right = 0;
    }
    if (keystate[SDLK_RIGHT]) {
        player->position.x += PLAYER_SPEED;
        isMoving = 1;
        player->facing_right = 1;
    }
    player->moving = isMoving;

    if (keystate[SDLK_SPACE] && !player->jumping) {
        player->velocityY = -25;
        player->jumping = 1;
    }
}

void update_player(Player* player) {
    int prev_x = player->position.x;
    int prev_y = player->position.y;

    player->velocityY += GRAVITY;
    if (player->velocityY > MAX_FALL_SPEED)
        player->velocityY = MAX_FALL_SPEED;
    player->position.y += player->velocityY;

    if (platform.active && check_collision(player->position, platform.position)) {
        int overlapLeft = (player->position.x + player->position.w) - platform.position.x;
        int overlapRight = (platform.position.x + platform.position.w) - player->position.x;
        int overlapTop = (player->position.y + player->position.h) - platform.position.y;
        int overlapBottom = (platform.position.y + platform.position.h) - player->position.y;

        int xOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
        int yOverlap = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

        if (xOverlap < yOverlap) {
            if (overlapLeft < overlapRight) {
                player->position.x = platform.position.x - player->position.w;
                if (player->moving && player->facing_right) {
                    player->position.x -= 20;
                }
            } else {
                player->position.x = platform.position.x + platform.position.w;
                if (player->moving && !player->facing_right) {
                    player->position.x += 20;
                }
            }
            if (game.health > 0) game.health -= 5;
        } else {
            if (overlapTop < overlapBottom) {
                player->position.y = platform.position.y - player->position.h;
                player->velocityY = 0;
                player->jumping = 0;
                if (game.health > 0) game.health -= 5;
            } else {
                player->position.y = platform.position.y + platform.position.h;
                player->velocityY = 0;
            }
        }
    }

    if (game.health <= 0) {
        game.health = MAX_HEALTH;
        load_game();
        return;
    }

    if (obstacle.active && check_collision(player->position, obstacle.position)) {
        int overlapLeft = (player->position.x + player->position.w) - obstacle.position.x;
        int overlapRight = (obstacle.position.x + obstacle.position.w) - player->position.x;
        int overlapTop = (player->position.y + player->position.h) - obstacle.position.y;
        int overlapBottom = (obstacle.position.y + obstacle.position.h) - player->position.y;

        int xOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
        int yOverlap = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

        if (xOverlap < yOverlap) {
            if (overlapLeft < overlapRight) {
                player->position.x = obstacle.position.x - player->position.w;
                if (player->moving && player->facing_right) {
                    player->position.x -= 20;
                }
            } else {
                player->position.x = obstacle.position.x + obstacle.position.w;
                if (player->moving && !player->facing_right) {
                    player->position.x += 20;
                }
            }
        } else {
            if (overlapTop < overlapBottom) {
                player->position.y = obstacle.position.y - player->position.h;
                player->velocityY = 0;
                player->jumping = 0;
                player->position.x += obstacle.velocityX;
            } else {
                player->position.y = obstacle.position.y + obstacle.position.h;
                player->velocityY = 0;
            }
        }
    }

    if (player->position.y + player->position.h >= GROUND_LEVEL) {
        player->position.y = GROUND_LEVEL - player->position.h;
        player->velocityY = 0;
        player->jumping = 0;
    }

    // Modified level transition logic
    if (player->position.x + player->position.w > SCREEN_WIDTH) {
        if(current_level == 2) { // Block right edge in City 3
            player->position.x = SCREEN_WIDTH - player->position.w;
        }
        else if(current_level < MAX_LEVELS-1) {
            load_level(current_level + 1);
            player->position.x = 0;
        } else {
            player->position.x = SCREEN_WIDTH - player->position.w;
        }
    } 
    else if (player->position.x < 0) {
        if(current_level > 0) {
            load_level(current_level - 1);
            player->position.x = SCREEN_WIDTH - player->position.w;
        } else {
            player->position.x = 0;
        }
    }

    if (player->jumping) {
        player->state = JUMP;
    } else if (player->moving) {
        player->state = WALK;
    } else {
        player->state = IDLE;
    }

    Uint32 now = SDL_GetTicks();
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

    if(player->position.y < 0) {
        player->position.y = 0;
        player->velocityY = 0;
    }
    if(player->position.y + player->position.h > SCREEN_HEIGHT) {
        player->position.y = SCREEN_HEIGHT - player->position.h;
        player->velocityY = 0;
    }
}

void draw_player(Player* player, SDL_Surface* screen) {
    SDL_Surface* currentSprite = player->facing_right ? player->sprite : player->leftSprite;
    SDL_BlitSurface(currentSprite, &player->srcRect, screen, &player->position);
}

void cleanup_player(Player* player) {
    if (player->sprite) SDL_FreeSurface(player->sprite);
    if (player->leftSprite) SDL_FreeSurface(player->leftSprite);
}
