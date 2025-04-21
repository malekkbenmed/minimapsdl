#include <SDL/SDL.h>
#include "minimap.h"

#define SCREEN_W 800
#define SCREEN_H 600
#define SCALE_FACTOR 20  // 20% scaling
#define PLAYER_SPEED 5

int main() {
    SDL_Surface *screen = NULL;
    SDL_Event event;
    int running = 1;
    
    Minimap mini;
    SDL_Rect player = {100, 100, 32, 32};  // Player position/size
    SDL_Rect camera = {0, 0, SCREEN_W, SCREEN_H};  // Viewport
    Uint8 *keystate = SDL_GetKeyState(NULL);

    // SDL initialization
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_SWSURFACE);
    if(!screen) {
        fprintf(stderr, "Video mode setup failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize minimap system
    init_minimap(&mini, "minimap.png", "playericon.png");

    // Main game loop
    while(running) {
        // Event handling
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) running = 0;
        }

        // Player movement
        if(keystate[SDLK_LEFT]) player.x -= PLAYER_SPEED;
        if(keystate[SDLK_RIGHT]) player.x += PLAYER_SPEED;
        if(keystate[SDLK_UP]) player.y -= PLAYER_SPEED;
        if(keystate[SDLK_DOWN]) player.y += PLAYER_SPEED;

        // Camera tracking (center on player)
        camera.x = player.x - SCREEN_W/2;
        camera.y = player.y - SCREEN_H/2;

        // Camera bounds checking
        camera.x = camera.x < 0 ? 0 : camera.x;
        camera.y = camera.y < 0 ? 0 : camera.y;

        // Update minimap
        update_minimap_position(&mini, player, camera, SCALE_FACTOR);

        // Rendering
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        draw_minimap(&mini, screen);
        SDL_Flip(screen);
        
        SDL_Delay(16);  // ~60 FPS
    }

    // Cleanup
    free_minimap(&mini);
    SDL_Quit();
    return 0;
}
