#include <bitset>
#include <map>
#include "input.hpp"
#include <Eigen/Dense>
using namespace Eigen;

const Uint16 MAX_EVENT_FLAGS = 100;
static Vector2f mouse_position = {0, 0};                // Mouse position relative to game view

static std::bitset<MAX_EVENT_FLAGS> event_flags;        // (EVENT_FLAG), this is global
static std::bitset<SDL_SCANCODE_COUNT> pressed_keys;    // Pressed Keys on this frame
static std::bitset<SDL_SCANCODE_COUNT> released_keys;   // Released Keys on this frame
static std::unordered_map<SDL_Scancode, bool> key_map;   // ScanCode, isPressed

bool input_handle_event() {
    pressed_keys.reset();
    released_keys.reset();

    SDL_Event event;

    // handle all events until none
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                return false;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) event_flags.set(MOUSE_LEFT_PRESSED, true);
                if (event.button.button == SDL_BUTTON_RIGHT) event_flags.set(MOUSE_RIGHT_PRESSED, true);
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) event_flags.set(MOUSE_LEFT_PRESSED, false);
                if (event.button.button == SDL_BUTTON_RIGHT) event_flags.set(MOUSE_RIGHT_PRESSED, false);
                break;

            case SDL_EVENT_KEY_DOWN:
                key_map[event.key.scancode] = true;
                pressed_keys.set(event.key.scancode);
                break;

            case SDL_EVENT_KEY_UP:
                key_map[event.key.scancode] = false;
                released_keys.set(event.key.scancode);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                mouse_position.x() = event.motion.x;
                mouse_position.y() = event.motion.y;
                break;

            default:
                break;
        }
    }

    return true;
}

bool check_key_released(SDL_Scancode code) {
    return released_keys[code];
}

bool check_key_pressed(SDL_Scancode code) {
    return pressed_keys[code];
}

bool check_key(SDL_Scancode code) {
    return key_map[code];
}

bool is_event_active(EVENT_FLAG flag) {
    return event_flags[flag];
}

void flip_event(EVENT_FLAG flag) {
    event_flags.flip(flag);
}

Vector2f& mouse_pos() {
    return mouse_position;
}