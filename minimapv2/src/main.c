#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"

int main(int argc, char* argv[]) {
    // Initialize game systems.
    init_game();
    init_player();
    init_objects();
    init_minimap();

    SDL_Event event;
    while(game.running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                game.running = 0;
            else if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_s)
                    save_game();
                else if(event.key.keysym.sym == SDLK_l)
                    load_game();
            }
        }
        
        update_player();
        update_objects();
        update_game();
        
        SDL_Delay(16);  // Roughly 60 FPS.
    }
    
    cleanup_game();
    return 0;
}

