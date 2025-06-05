#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL3/SDL.h>
#include <Eigen/Dense>
using namespace Eigen;

/**
 * @brief Flags representing various input events in the game.
 */
enum EVENT_FLAG {
    MOUSE_LEFT_PRESSED,   
    MOUSE_RIGHT_PRESSED,  
    DEBUG_MODE            
};


/**
 * @brief Handles input events and updates event flags.
 * @return False if the application should quit, true otherwise.
 */
bool input_handle_event();


/**
 * @brief Checks if a specific event flag is currently active.
 * @param flag The event flag to check.
 * @return True if the event is active, false otherwise.
 */
bool is_event_active(EVENT_FLAG flag);


/**
 * @brief Toggles the state of a specific event flag.
 * @param flag The event flag to flip.
 */
void flip_event(EVENT_FLAG flag);


/**
 * @brief Checks if a key was released this frame.
 * @param code The SDL_Scancode of the key.
 * @return True if the key was released, false otherwise.
 */
bool check_key_released(SDL_Scancode code);


/**
 * @brief Checks if a key was pressed this frame.
 * @param code The SDL_Scancode of the key.
 * @return True if the key was pressed, false otherwise.
 */
bool check_key_pressed(SDL_Scancode code);


/**
 * @brief Checks if a key is currently held down.
 * @param code The SDL_Scancode of the key.
 * @return True if the key is held, false otherwise.
 */
bool check_key(SDL_Scancode code);


/**
 * @brief Returns a reference to the current mouse position in screen coordinates.
 * @return Reference to a Vector2f representing the mouse position.
 */
Vector2f& mouse_pos();

#endif