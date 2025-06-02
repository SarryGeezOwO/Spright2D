#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL3/SDL.h>
#include <Eigen/Dense>
using namespace Eigen;

// Tracks down Event Flags needed inside of a game.
enum EVENT_FLAG {
    MOUSE_LEFT_PRESSED,
    MOUSE_RIGHT_PRESSED,
    DEBUG_MODE
};

// Returns false if the app should quit
bool input_handle_event();
bool is_event_active(EVENT_FLAG flag);
void flip_event(EVENT_FLAG flag);

bool check_key_released(SDL_Scancode code);
bool check_key_pressed(SDL_Scancode code);
bool check_key(SDL_Scancode code);
Vector2f& mouse_pos();
#endif