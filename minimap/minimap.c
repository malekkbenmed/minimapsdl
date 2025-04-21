#include "minimap.h"
#include <SDL/SDL_image.h>
#include <stdlib.h>

#define CLAMP(val, min, max) ((val) < (min) ? (min) : (val) > (max) ? (max) : (val))

void init_minimap(Minimap *m, const char *map_path, const char *icon_path) {
    // Load images with error checking
    m->map = IMG_Load(map_path);
    if(!m->map) {
        fprintf(stderr, "Failed to load minimap: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    
    m->player_icon = IMG_Load(icon_path);
    if(!m->player_icon) {
        fprintf(stderr, "Failed to load player icon: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    // Optimize surfaces for current display
    SDL_Surface *screen = SDL_GetVideoSurface();
    m->map = SDL_ConvertSurface(m->map, screen->format, 0);
    m->player_icon = SDL_ConvertSurface(m->player_icon, screen->format, 0);

    // Position minimap in top-left corner
    m->map_pos.x = 20;
    m->map_pos.y = 20;
}

void update_minimap_position(Minimap *m, SDL_Rect player_world_pos, 
                           SDL_Rect camera, int scale) {
    // Calculate absolute world position
    SDL_Rect abs_pos = {
        .x = player_world_pos.x + camera.x,
        .y = player_world_pos.y + camera.y
    };

    // Calculate scaled position
    int icon_x = m->map_pos.x + (abs_pos.x * scale) / 100;
    int icon_y = m->map_pos.y + (abs_pos.y * scale) / 100;

    // Keep icon within minimap bounds
    m->icon_pos.x = CLAMP(icon_x, 
        m->map_pos.x, 
        m->map_pos.x + m->map->w - m->player_icon->w
    );
    
    m->icon_pos.y = CLAMP(icon_y,
        m->map_pos.y,
        m->map_pos.y + m->map->h - m->player_icon->h
    );
}

void draw_minimap(Minimap *m, SDL_Surface *screen) {
    SDL_BlitSurface(m->map, NULL, screen, &m->map_pos);
    SDL_BlitSurface(m->player_icon, NULL, screen, &m->icon_pos);
}

void free_minimap(Minimap *m) {
    SDL_FreeSurface(m->map);
    SDL_FreeSurface(m->player_icon);
}
