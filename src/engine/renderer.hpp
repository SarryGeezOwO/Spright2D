#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL3/SDL.h>
#include <Eigen/Dense>
using namespace Eigen;


// Variable declarion of four SDL_Vertex [TL, TR, BR, BL]
#define QUAD SDL_Vertex v0, v1, v2, v3
#define MAX_VERTEX_COUNT UINT16_MAX
struct Entity;
struct Camera;

struct VertexBuffer {
    SDL_Vertex  vertices[MAX_VERTEX_COUNT];
    int         indices[MAX_VERTEX_COUNT / 2 * 6];
    Uint16      vert_count;
    Uint16      index_count;
};

void render_init(SDL_Renderer* rend);

// Insert an Entity to the rendering batch
void render_batch_entity(Entity& entity, Camera const cam);

// Insert a sprite to the rendering batch
void render_batch_sprite(std::string sprite_id, Uint8 index, float rotation, Vector2f scale, Uint16 depth, std::array<Vector2f, 4>& vertices, Camera const cam);

// Draws all VertexBuffer loaded from the batch, ordered by depth value
// Higher depth means closer to the screen
void render_batch_all(bool debug);
void render_batch_clear_all(); 
int& rendered_count();

#endif