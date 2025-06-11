#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "camera.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <Eigen/Dense>
using namespace Eigen;


/**
 * @brief Stores metadata and properties for a sprite sheet.
 * 
 * All sprites have Top-Left pivot points.
 * This pivot point is unrelated to rendering and entites.
 * They have their own ways of defining pivot point.
 */
struct Sprite_sheet_data {
    Uint64 sprite_id;           /**< The HashID of a sprite sheet. */
    std::string sprite_name;    /**< The name of a sprite sheet. */
    int frame_count;            /**< Number of frames in the sprite sheet (auto-calculated). */
    int fps;                    /**< Frames per second for animation (defaults to 30 fps). */
    Vector2i location;          /**< Location of the sprite sheet on the texture atlas. */
    Vector2i frame_size;        /**< Size of each frame in the sprite sheet (auto-calculated). */
    Vector4f UV_coord;          /**< UV coordinates of the sprite sheet, from top-left to bottom-right. */
    bool loop;                  /**< Whether the animation should loop (true by default). */

    /**
     * @brief Calculates the total size of the sprite sheet.
     * @return The width and height of the entire sprite sheet.
     */
    Vector2i sheet_size() const {
        return Vector2i(frame_size.x() * frame_count, frame_size.y());
    }
};


/**
 * @brief Loads all available sprites inside the asset folder.
 * @param rend Pointer to the SDL_Renderer used for loading textures.
 */
void init_sprite_manager(SDL_Renderer* rend);


/**
 * @brief Adds a sprite to the sprite manager.
 * 
 * sprite_file follows this naming convention: <<spr_name_5.png>> where '5' is the number of frames
 * @param sprite_file The file name of the sprite under "assets/sprites" directory.
 * @param fps Frames per second for the sprite animation.
 */
void sprite_add(const std::string sprite_file, int fps);


/**
 * @brief Cleans up and releases all sprite resources.
 */
void sprite_cleanup();


/**
 * @brief Gets the rectangle for a specific frame of a sprite.
 * @param sprite_id The ID of the sprite.
 * @param index The frame index.
 * @return SDL_FRect representing the frame's position and size.
 */
SDL_FRect sprite_frame_at(const Uint64 sprite_id, Uint16 index);


/**
 * @brief Gets the UV coordinates for a specific frame of a sprite.
 * @param sprite_id The ID of the sprite.
 * @param index The frame index.
 * @return Vector4f containing the UV coordinates [Top-Left, Bottom-Right].
 */
Vector4f sprite_frame_at_uv(const Uint64 sprite_id, Uint16 index);


/**
 * @brief Gets the texture atlas containing all sprites.
 * @return Pointer to the SDL_Texture atlas.
 */
SDL_Texture* sprite_get_atlas();


/**
 * @brief Retrieves the Sprite_sheet_data for a given sprite.
 * @param sprite_name The name of the sprite.
 * @return Reference to the Sprite_sheet_data.
 */
Sprite_sheet_data& sprite_get(const std::string& sprite_name);


/**
 * @brief Retrieves the Sprite_sheet_data for a given sprite.
 * @param sprite_id The ID of the sprite.
 * @return Reference to the Sprite_sheet_data.
 */
Sprite_sheet_data& sprite_get(const Uint64 sprite_id);


/**
 * @brief Returns the total number of loaded sprites.
 * @return The number of sprites.
 */
int sprite_count();


// ============= GPU ==================== //


/**
 * @brief Draws a sprite directly from the GPU without batching.
 * @param sprite_id The ID of the sprite to draw.
 * @param index The frame index.
 * @param rotation Rotation in degrees.
 * @param cam The camera used for rendering transformations.
 * @param rect The destination rectangle for drawing.
 */
void draw_sprite(const Uint64 sprite_id, Uint8 index, float rotation, Camera const cam, SDL_FRect rect);


/**
 * @brief Draws a sprite directly from the GPU without batching (no camera).
 * @param sprite_id The ID of the sprite to draw.
 * @param index The frame index.
 * @param rotation Rotation in degrees.
 * @param rect The destination rectangle for drawing.
 */
void draw_sprite_raw(const Uint64 sprite_id, Uint8 index, float rotation, SDL_FRect rect);

#endif