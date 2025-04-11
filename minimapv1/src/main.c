#include "game.h"
#include "player.h"
#include "objects.h"
#include "minimap.h"

int main(int argc, char* argv[]) {
    // Initialize game systems and resources.
    init_game();
    init_player();
    init_objects();
    init_minimap();

    SDL_Event event;
    while(game.running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                game.running = 0;
        }
        
        // Update logic: player movement (including jump) and objects.
        update_player();
        update_objects();
        
        // Render everything to the screen.
        update_game();
        
        SDL_Delay(16);  // Roughly 60 frames per second.
    }
    
    // Clean up resources before exiting.
    cleanup_game();
    return 0;
}

