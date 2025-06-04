#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "sprite.hpp"
#include "camera.hpp"
#include <Eigen/Dense>
#include <functional>
#include <array>
#include <map>
using namespace Eigen;

static const Uint16 MAX_ENTITIES = 1000;

// Position is based on Top-Left
struct Entity {
    int id;                   // The Index location on the Entity Array
    Uint16 depth;             // Entity Depth draw, Defaults to 100
    Sprite_sheet_data sprite; // The Sprite_sheet to refer to
    Uint8 image_index;        // The current frame of the sprite
    Uint64 last_frame_time;   // Tracking time for FPS
    std::array<Vector2f, 4> vertices;

    // Transform
    Vector2f position;      // World Position
    Vector2f scale;
    float rotation;

    Vector2f center_pivot() const {
        return {
            (scale.x() * sprite.frame_size.x()) / 2,
            (scale.y() * sprite.frame_size.y()) / 2,
        };
    }

    Vector2f center() const {
        return {
            position.x() + (scale.x() * sprite.frame_size.x() / 2),
            position.y() + (scale.y() * sprite.frame_size.y() / 2),
        };
    }

    void update_vertices() {
        vertices[0] = position;                                                                                                        // Top-left
        vertices[1] = Vector2f(position.x() + (scale.x() * sprite.frame_size.x()), position.y());                                       // Top-right
        vertices[2] = Vector2f(position.x() + (scale.x() * sprite.frame_size.x()), position.y() + (scale.y() * sprite.frame_size.y())); // Bottom-right
        vertices[3] = Vector2f(position.x(), position.y() + (scale.y() * sprite.frame_size.y()));                                        // Bottom-left
    }

    void update_frame(Uint16 now) {
        if ((now - last_frame_time) >= (1000.0f / sprite.fps)) {
            image_index = (image_index + 1) % sprite.frame_count;
            last_frame_time = now;
        }
    }

    void submit_vertices(const SDL_Renderer* rend, Camera& cam) {
        // Submit Vertex buffer and Texture
        batch_draw_sprite(
            sprite.sprite_id,
            image_index, rotation, depth,
            vertices, cam
        );
    }
};

int entity_spawn(std::string sprite, Vector2f pos, Vector2f scale, float rotation, Uint16 depth);
void entity_destroy(Uint16 id);
Entity& entity_get(Uint16 id);
int entity_count();

std::unordered_map<Uint16, Entity>& entity_get_map();
#endif