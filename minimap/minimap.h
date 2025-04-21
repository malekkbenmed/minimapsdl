#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL/SDL.h>

typedef struct {
    SDL_Surface *map;          // Minimap background surface
    SDL_Surface *player_icon;  // Player indicator surface
    SDL_Rect map_pos;          // Screen position of minimap
    SDL_Rect icon_pos;         // Player position in minimap
} Minimap;

// Initializes minimap resources
void init_minimap(Minimap *m, const char *map_path, const char *icon_path);

// Updates player position in minimap
void update_minimap_position(Minimap *m, SDL_Rect player_world_pos, 
                           SDL_Rect camera, int scale);

// Renders minimap to screen
void draw_minimap(Minimap *m, SDL_Surface *screen);

// Frees minimap resources
void free_minimap(Minimap *m);

#endif
