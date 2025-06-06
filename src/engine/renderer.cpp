#include "renderer.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "sprite.hpp"

#include <map>
#include <SDL3/SDL.h>
#include <Eigen/Dense>
using namespace Eigen;

static SDL_Renderer* renderer = nullptr;
static std::map<Uint16, VertexBuffer> render_batches;  // Depth, VertexBuffer
static int prev_rend_c = 0;
static int rendered_c = 0;


void render_init(SDL_Renderer* rend) {
    renderer = rend;
}

void add_quad_to_batch(VertexBuffer& buf, SDL_Vertex v0, SDL_Vertex v1, SDL_Vertex v2, SDL_Vertex v3) {
    Uint16& c   = buf.vert_count;
    Uint16& i   = buf.index_count;

    buf.indices[i++] = c + 0;
    buf.indices[i++] = c + 1;
    buf.indices[i++] = c + 2;
    buf.indices[i++] = c + 2;
    buf.indices[i++] = c + 3;
    buf.indices[i++] = c + 0;

    buf.vertices[c++] = v0;
    buf.vertices[c++] = v1;
    buf.vertices[c++] = v2;
    buf.vertices[c++] = v3;
    rendered_c++;
}


void render_batch_entity(const Entity& entity, const Camera& cam) {
    // Render if at least one vertex is seenable
    if (camera_is_quad_in(cam, entity.bbox())) return;

    SDL_Vertex v0, v1, v2, v3;
    Vector2f tl = world_to_screen(cam, entity.transformed_vertices[0]);
    Vector2f tr = world_to_screen(cam, entity.transformed_vertices[1]);
    Vector2f br = world_to_screen(cam, entity.transformed_vertices[2]);
    Vector2f bl = world_to_screen(cam, entity.transformed_vertices[3]);
    Vector4f uv = sprite_frame_at_uv(entity.sprite.sprite_id, entity.image_index);

    // Top left
    v0.position.x = tl.x();
    v0.position.y = tl.y();
    v0.tex_coord.x = uv.x();
    v0.tex_coord.y = uv.y();
    v0.color = {1, 1, 1, 1}; 

    // Top Right
    v1.position.x = tr.x();
    v1.position.y = tr.y();
    v1.tex_coord.x = uv.z();
    v1.tex_coord.y = uv.y();
    v1.color = {1, 1, 1, 1};

    // Bottom right
    v2.position.x = br.x();
    v2.position.y = br.y();
    v2.tex_coord.x = uv.z();
    v2.tex_coord.y = uv.w();
    v2.color = {1, 1, 1, 1};
    
    // Bottom left
    v3.position.x = bl.x();
    v3.position.y = bl.y();
    v3.tex_coord.x = uv.x();
    v3.tex_coord.y = uv.w();
    v3.color = {1, 1, 1, 1};
    add_quad_to_batch(render_batches[entity.depth], v0, v1, v2, v3);
}


void render_batch_sprite(std::string sprite_id, Uint8 index, float rotation, Vector2f scale, 
    Uint16 depth, const std::array<Vector2f, 4>& vertices, const Camera& cam) {
    Vector4f bbox = {
        vertices[0].x(),
        vertices[0].y(),
        vertices[2].x(),
        vertices[2].y()
    };

    // Render if at least one vertex is seenable
    if (camera_is_quad_in(cam, bbox)) return;

    SDL_Vertex v0, v1, v2, v3;
    Vector2f tl = world_to_screen(cam, vertices[0]);
    Vector2f tr = world_to_screen(cam, vertices[1]);
    Vector2f br = world_to_screen(cam, vertices[2]);
    Vector2f bl = world_to_screen(cam, vertices[3]);
    Vector4f uv = sprite_frame_at_uv(sprite_id, index);

    // TODO: apply transformation matrix here (rotation and Scale)


    // Top left
    v0.position.x = tl.x();
    v0.position.y = tl.y();
    v0.tex_coord.x = uv.x();
    v0.tex_coord.y = uv.y();
    v0.color = {1, 0, 1, 1}; 

    // Top Right
    v1.position.x = tr.x();
    v1.position.y = tr.y();
    v1.tex_coord.x = uv.z();
    v1.tex_coord.y = uv.y();
    v1.color = {1, 1, 0, 1};

    // Bottom right
    v2.position.x = br.x();
    v2.position.y = br.y();
    v2.tex_coord.x = uv.z();
    v2.tex_coord.y = uv.w();
    v2.color = {0, 1, 1, 1};
    
    // Bottom left
    v3.position.x = bl.x();
    v3.position.y = bl.y();
    v3.tex_coord.x = uv.x();
    v3.tex_coord.y = uv.w();
    v3.color = {1, 1, 1, 1};
    add_quad_to_batch(render_batches[depth], v0, v1, v2, v3);
}


// Draws all sprite loaded from the batch with all Depth value
void render_batch_all(bool debug) {

    // For each Sprite_id request
    for (auto& [depth, batch] : render_batches) {

        // Render using the texture corresponding to this depth batch
        bool f = SDL_RenderGeometry(
            renderer, 
            sprite_get_atlas(), 
            batch.vertices, 
            batch.vert_count, 
            batch.indices, 
            batch.index_count
        );

        if (debug) {
            SDL_FPoint debug_points[batch.index_count];
            for (int i = 0; i < batch.index_count; i++) {
                debug_points[i] = batch.vertices[batch.indices[i]].position;
            }
            SDL_RenderPoints(renderer, debug_points, batch.index_count);
        }

        if (!f) {
            SDL_Log("Vert_count: %d", batch.vert_count);
            SDL_Log("Idx_count: %d", batch.index_count);
            SDL_Log("Error: {%s}", SDL_GetError());
        }
    }
}


void render_batch_clear_all() {
    render_batches.clear();
    prev_rend_c = rendered_c;
    rendered_c = 0;
}


int& rendered_count() {
    return rendered_c;
}