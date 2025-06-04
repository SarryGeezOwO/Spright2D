#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "camera.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <Eigen/Dense>
using namespace Eigen;

/* All sprites have Top-Left pivot points */

struct Sprite_sheet_data {
    std::string sprite_id;      // File name
    int frame_count;            // Auto-calculated
    int fps;                    // Educated guess by default
    Vector2i location;          // Location on the Texture atlas
    Vector2i frame_size;        // Auto-calculated
    Vector4f UV_coord;          // The UV coordinate of this sprite sheet, TL to BR
    bool loop;                  // True by default

    Vector2i sheet_size() const {
        return Vector2i(frame_size.x() * frame_count, frame_size.y());
    }
};

// Load all available sprites inside the asset folder
void init_sprite_manager(SDL_Renderer* rend);
void sprite_add(std::string sprite_id, int fps);
void sprite_cleanup();

SDL_FRect sprite_frame_at(std::string sprite_id, Uint16 index);
Vector4f sprite_frame_at_uv(std::string sprite_id, Uint16 index);
SDL_Texture* sprite_get_atlas();

Sprite_sheet_data& sprite_get(std::string sprite_id);
int sprite_count();

// ============= GPU ==================== //

// Draws directly from the GPU without batching
void draw_sprite(std::string sprite_id, Uint8 index, float rotation, Camera const cam, SDL_FRect rect);

// Draws directly from the GPU without batching
void draw_sprite_raw(std::string sprite_id, Uint8 index, float rotation, SDL_FRect rect);

#endif 