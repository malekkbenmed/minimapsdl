#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    init_game();

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
    while (game.running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                game.running = 0;
            else if (event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_s)
                    save_game();
                else if(event.key.keysym.sym == SDLK_l)
                    load_game();
                else if(event.key.keysym.sym == SDLK_e) {
                    if(current_level == 2) { // City3 subway entrance
                        SDL_Rect subwayArea = {545, 450, 50, 50};
                        if (check_collision(player.position, subwayArea)) {
                            load_level(3);
                        }
                    }
                    else if(current_level == 3) { // City4 subway exit
                        SDL_Rect exitSubwayArea = {380, 465, 50, 50};
                        if (check_collision(player.position, exitSubwayArea)) {
                            load_level(2);
                        }
                    }
                }
            }
        }

        const Uint8* keystate = SDL_GetKeyState(NULL);
        handle_input_player(&player, keystate);
        update_player(&player); 
        update_objects();
        update_game();
        SDL_Flip(game.screen);
        SDL_Delay(16);
    }

    cleanup_game();
    return 0;
}
