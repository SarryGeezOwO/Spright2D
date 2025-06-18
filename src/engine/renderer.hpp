#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "../utils/util.hpp"
#include "geometry.hpp"
#include <vector>
#include <SDL3/SDL.h>
#include <Eigen/Dense>
using namespace Eigen;

#define MAX_VERTEX_COUNT UINT16_MAX
struct Entity;
struct Camera;

/**
 * @brief Stores vertex and index data for rendering.
 *        This buffer represents a whole depth
 */
struct VertexBuffer {
    SDL_Vertex  vertices[MAX_VERTEX_COUNT];   /**< Array of vertices for rendering. */
    int         indices[MAX_VERTEX_COUNT / 2 * 6]; /**< Indices for indexed drawing. */
    Uint16      vert_count;                   /**< Number of vertices currently stored. */
    Uint16      index_count;                  /**< Number of indices currently stored. */
};


/**
 * @brief Initializes the renderer with the given SDL_Renderer.
 * 
 * @param rend Pointer to the SDL_Renderer to use for all drawing.
 */
void render_init(SDL_Renderer* rend);


// ALl draw calls will submit their vertices, appropriately
// is_primitive if false, tells this function that a quad is requested because it needs texture
// meaning primitive draw calls can't support texutes... I know I am bad at this shit
void render_submit_vertices(const SDL_Vertex vertices[], const std::vector<int>& indices, int vert_count, Uint16 depth, bool is_primitive); 


/**
 * @brief Adds an Entity to the rendering batch.
 * 
 * @param entity Reference to the Entity to batch.
 * @param cam The camera used for rendering transformations.
 */
void render_batch_entity(const Entity& entity, const Camera& cam);


/**
 * @brief Adds a sprite to the rendering batch.
 * 
 * @param sprite_id The ID of the sprite to render.
 * @param index The sprite frame or sub-image index.
 * @param rotation Rotation in radians.
 * @param scale Scaling factor for the sprite.
 * @param depth Rendering depth (higher = closer to screen).
 * @param vertices Array of 4 vertices defining the sprite quad.
 * @param cam The camera used for rendering transformations.
 */
void render_batch_sprite(const Uint64 sprite_id, Uint8 index, float rotation, Vector2f scale, Uint16 depth, const std::array<Vector2f, 4>& vertices, const Camera& cam);


// =========== PRIMITIVE SHAPES =================== //


/**
 * @brief Renders a circle centered at the given position.
 * 
 * @param position Center position in world coordinates.
 * @param radius Radius of the circle.
 * @param precision Number of vertices to approximate the circle (higher = smoother).
 * @param depth Rendering depth.
 */
void render_circle(Vector2f position, int radius, int precision, Uint16 depth);


/**
 * @brief Renders a rectangle at the given position and size.
 * 
 * @param position Position in world coordinates.
 * @param size Width and height of the rectangle.
 * @param pivot Pivot type for positioning (e.g., center, top-left).
 * @param depth Rendering depth.
 */
void render_rectangle(Vector2f position, Vector2f size, Pivot_Type pivot, Uint16 depth);


/**
 * @brief Renders a line between two points with specified thickness.
 * 
 * @param a Start point.
 * @param b End point.
 * @param thickness Thickness of the line.
 * @param depth Rendering depth.
 */
void render_line(Vector2f a, Vector2f b, int thickness, Uint16 depth);


template<size_t N>
void render_polygon(Polygon<N> polygon, Uint16 depth) {

}

// =========================================================== //

void set_color(const SDL_Color& color);
void set_color(const SDL_FColor& color);

/**
 * @brief Draws all VertexBuffers loaded from the batch, ordered by depth.
 * 
 * @param debug If true, enables debug rendering (e.g., outlines, diagnostics).
 */
void render_batch_all(bool debug);


/**
 * @brief Clears all rendering batches.
 */
void render_batch_clear_all();


/**
 * @brief Returns a reference to the count of rendered objects in the current frame.
 * 
 * @return Reference to the rendered count integer.
 */
int& rendered_count();

#endif