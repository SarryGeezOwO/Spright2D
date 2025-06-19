#include "renderer.hpp"
#include "geometry.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "sprite.hpp"

#include <map>
#include <bits/stdc++.h>
#include <SDL3/SDL.h>
#include <Eigen/Dense>
using namespace Eigen;

static SDL_Renderer* renderer = nullptr;
static std::map<Uint16, std::pair<VertexBuffer, VertexBuffer>> render_batches;  // Depth, <VertexBuffer(Textured), VertexBuffer(Primitive)>
static int prev_rend_c = 0;
static int rendered_c = 0;


void render_init(SDL_Renderer* rend) {
    renderer = rend;
}


void render_submit_vertices(const SDL_Vertex vertices[], const std::vector<int>& indices, int vert_count, Uint16 depth, bool is_primitive) {
    int idx_count = indices.size();
    std::pair<VertexBuffer, VertexBuffer>& buf = render_batches[depth];

    if (is_primitive) 
    {
        Uint16& cc   = buf.second.vert_count;
        Uint16& ii   = buf.second.index_count;
        // For each indices
        for (int i = 0; i < indices.size(); i++) {
            buf.second.indices[ii++] = cc + indices[i];
        }

        // For each vertex
        for (int i = 0; i < vert_count; i++) {
            buf.second.vertices[cc++] = vertices[i];
        }
    }
    else 
    {
        // literally just a quad, hopefully lol (no test cases aahhh type shit...)
        Uint16& c   = buf.first.vert_count;
        Uint16& i   = buf.first.index_count;

        buf.first.indices[i++] = c + 0;
        buf.first.indices[i++] = c + 1;
        buf.first.indices[i++] = c + 2;
        buf.first.indices[i++] = c + 2;
        buf.first.indices[i++] = c + 3;
        buf.first.indices[i++] = c + 0;

        buf.first.vertices[c++] = vertices[0];
        buf.first.vertices[c++] = vertices[1];
        buf.first.vertices[c++] = vertices[2];
        buf.first.vertices[c++] = vertices[3];
    }

    rendered_c++;
}


void render_batch_entity(const Entity& entity, const Camera& cam) {
    // Render if at least one vertex is seenable
    Polygon<4> ent_shape(entity.transformed_vertices);
    if (!camera_is_polygon_in(cam, ent_shape)) return;

    SDL_Vertex vertices[4];
    Vector2f tl = world_to_screen(cam, ent_shape[0]);
    Vector2f tr = world_to_screen(cam, ent_shape[1]);
    Vector2f br = world_to_screen(cam, ent_shape[2]);
    Vector2f bl = world_to_screen(cam, ent_shape[3]);
    Vector4f uv = sprite_frame_at_uv(entity.sprite.sprite_id, entity.image_index);

    // Top left
    vertices[0].position.x = tl.x();
    vertices[0].position.y = tl.y();
    vertices[0].tex_coord.x = uv.x();
    vertices[0].tex_coord.y = uv.y();
    vertices[0].color = entity.c_blend; 

    // Top Right
    vertices[1].position.x = tr.x();
    vertices[1].position.y = tr.y();
    vertices[1].tex_coord.x = uv.z();
    vertices[1].tex_coord.y = uv.y();
    vertices[1].color = entity.c_blend;

    // Bottom right
    vertices[2].position.x = br.x();
    vertices[2].position.y = br.y();
    vertices[2].tex_coord.x = uv.z();
    vertices[2].tex_coord.y = uv.w();
    vertices[2].color = entity.c_blend;
    
    // Bottom left
    vertices[3].position.x = bl.x();
    vertices[3].position.y = bl.y();
    vertices[3].tex_coord.x = uv.x();
    vertices[3].tex_coord.y = uv.w();
    vertices[3].color = entity.c_blend;

    std::vector<int> indices;
    render_submit_vertices(vertices, indices, 4, entity.depth, false);
}


void render_batch_sprite(const Uint64 sprite_id, Uint8 index, float rotation, Vector2f scale, 
    Uint16 depth, const std::array<Vector2f, 4>& vertices, const Camera& cam) {
    // Render if at least one vertex is seenable
    Polygon<4> ent_shape(vertices);
    if (!camera_is_polygon_in(cam, ent_shape)) return;

    SDL_Vertex vertices_sdl[4];
    Vector2f tl = world_to_screen(cam, ent_shape[0]);
    Vector2f tr = world_to_screen(cam, ent_shape[1]);
    Vector2f br = world_to_screen(cam, ent_shape[2]);
    Vector2f bl = world_to_screen(cam, ent_shape[3]);
    Vector4f uv = sprite_frame_at_uv(sprite_id, index);

    // TODO: apply transformation matrix here (rotation and Scale)

    // Top left
    vertices_sdl[0].position.x = tl.x();
    vertices_sdl[0].position.y = tl.y();
    vertices_sdl[0].tex_coord.x = uv.x();
    vertices_sdl[0].tex_coord.y = uv.y();
    vertices_sdl[0].color = {1, 1, 1, 1}; 

    // Top Right
    vertices_sdl[1].position.x = tr.x();
    vertices_sdl[1].position.y = tr.y();
    vertices_sdl[1].tex_coord.x = uv.z();
    vertices_sdl[1].tex_coord.y = uv.y();
    vertices_sdl[1].color = {1, 1, 1, 1};

    // Bottom right
    vertices_sdl[2].position.x = br.x();
    vertices_sdl[2].position.y = br.y();
    vertices_sdl[2].tex_coord.x = uv.z();
    vertices_sdl[2].tex_coord.y = uv.w();
    vertices_sdl[2].color = {1, 1, 1, 1};
    
    // Bottom left
    vertices_sdl[3].position.x = bl.x();
    vertices_sdl[3].position.y = bl.y();
    vertices_sdl[3].tex_coord.x = uv.x();
    vertices_sdl[3].tex_coord.y = uv.w();
    vertices_sdl[3].color = {1, 1, 1, 1};

    std::vector<int> indices;
    render_submit_vertices(vertices_sdl, indices, 4, depth, false);
}


// Draws all sprite loaded from the batch with all Depth value
void render_batch_all(bool debug) {

    // For each Sprite_id request
    for (auto& [depth, batch] : render_batches) {

        // Render using the texture corresponding to this depth batch
        SDL_RenderGeometry(         // Textured
            renderer, 
            sprite_get_atlas(), 
            batch.first.vertices, 
            batch.first.vert_count, 
            batch.first.indices, 
            batch.first.index_count
        );

        SDL_RenderGeometry(         // Primitives
            renderer, 
            sprite_get_atlas(), 
            batch.second.vertices, 
            batch.second.vert_count, 
            batch.second.indices, 
            batch.second.index_count
        );
    }
}

void set_color(const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void set_color(const SDL_FColor& color) {
    SDL_SetRenderDrawColorFloat(renderer, color.r, color.g, color.b, color.a);
}

void render_batch_clear_all() {
    render_batches.clear();
    prev_rend_c = rendered_c;
    rendered_c = 0;
}


int& rendered_count() {
    return rendered_c;
}