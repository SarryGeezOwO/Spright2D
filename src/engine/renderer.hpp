#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "../utils/util.hpp"
#include <SDL3/SDL.h>
#include <Eigen/Dense>
using namespace Eigen;

#define MAX_VERTEX_COUNT UINT16_MAX
struct Entity;
struct Camera;

/**
 * @brief Stores vertex and index data for rendering.
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
 * @brief Renders a triangle defined by three vertices.
 * 
 * @param v0 First vertex.
 * @param v1 Second vertex.
 * @param v2 Third vertex.
 * @param depth Rendering depth.
 */
void render_triangle(Vector2f v0, Vector2f v1, Vector2f v2, Uint16 depth);


/**
 * @brief Renders a line between two points with specified thickness.
 * 
 * @param a Start point.
 * @param b End point.
 * @param thickness Thickness of the line.
 * @param depth Rendering depth.
 */
void render_line(Vector2f a, Vector2f b, int thickness, Uint16 depth);


// ========== CUSTOM SHAPES ==================== //


/**
 * @brief Renders a custom shape from an array of SDL_Vertex.
 * 
 * @param vertices Pointer to the array of vertices (must be terminated or sized elsewhere).
 * @param depth Rendering depth.
 */
void render_shape(SDL_Vertex* vertices, Uint16 depth);


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