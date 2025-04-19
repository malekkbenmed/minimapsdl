#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL/SDL.h>

typedef struct {
    SDL_Surface* sprite;      // The sprite for the object
    SDL_Rect position;        // Position of the object
    int active;               // Whether the object is active or not

    // Animation-related variables (added here)
    int frame;                // Current frame of animation
    int max_frame;            // Maximum number of frames for the animation
    int velocityX;            // Horizontal velocity of the object
    int leftLimit;            // Left limit for obstacle movement
    int rightLimit;           // Right limit for obstacle movement
} GameObject;

// Declare external objects
extern GameObject platform;
extern GameObject coin;
extern GameObject obstacle;

void init_objects();
void update_objects();
void draw_objects();
int check_collision(SDL_Rect a, SDL_Rect b);

#endif // OBJECTS_H

