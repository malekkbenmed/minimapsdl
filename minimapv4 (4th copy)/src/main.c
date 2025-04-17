#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    init_game();

    // Load player sprite with transparency
    SDL_Surface* playerSprite = IMG_Load("assets/player.png");
    if (!playerSprite) {
        fprintf(stderr, "Failed to load player.png: %s\n", IMG_GetError());
        cleanup_game();
        exit(1);
    }
    SDL_SetColorKey(playerSprite, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(playerSprite->format, 255, 255, 255));
    playerSprite = SDL_DisplayFormat(playerSprite);

    init_player(&player, playerSprite);
    init_objects();
    init_minimap();

    SDL_Event event;
    Uint32 lastTicks = SDL_GetTicks();

    while (game.running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                game.running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_s)
                    save_game();
                else if(event.key.keysym.sym == SDLK_l)
                    load_game();
            }
        }
        
        // Compute delta time (in milliseconds)
        Uint32 currentTicks = SDL_GetTicks();
        Uint32 deltaTime = currentTicks - lastTicks;
        lastTicks = currentTicks;

        // Get current keystate and update player input
        const Uint8* keystate = SDL_GetKeyState(NULL);
        handle_input_player(&player, keystate);
        // Pass deltaTime to update functions (see next section)
        update_player(&player, deltaTime);
        update_objects(deltaTime);
        update_game();
        
        SDL_Flip(game.screen);
        
        // Optionally, add a short delay to reduce CPU usage
        SDL_Delay(1);
    }

    cleanup_game();
    return 0;
}

