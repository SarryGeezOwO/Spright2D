#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "sprite.hpp"
#include "camera.hpp"
#include "renderer.hpp"

#include "../utils/util.hpp"
#include <Eigen/Dense>
#include <functional>
#include <array>
#include <map>
using namespace Eigen;
static const Uint16 MAX_ENTITIES = 1000;

struct Entity {
    int id;                          // The Index location on the Entity Array
    Pivot_Type pivot = TOP_LEFT;     // The point where position rests, Defaults to TOP_LEFT
    Sprite_sheet_data sprite;        // The Sprite_sheet to refer to
    Uint8 image_index;               // The current frame of the sprite
    Uint64 last_frame_time;          // Tracking time for FPS
    std::array<Vector2f, 4> vertices;               // If you need the original points of this entity without it's transformation ( no rotation and Scale )
    std::array<Vector2f, 4> transformed_vertices;   // The vertices with applied Rotation, Scale

    // Transform
    Uint16 depth;                    // Entity Depth draw, Defaults to 100
    Vector2f position;               // World Position
    Vector2f scale;
    float rotation;                 // In Degrees

    void update_vertices() {
        Vector2f size = Vector2f{scale.x() * sprite.frame_size.x(), scale.y() * sprite.frame_size.y()};
        Vector2f offset = get_pivot_offset(pivot, size);
        vertices[0] = Vector2f(position.x() - offset.x(), position.y() - offset.y());   // Top-left
        vertices[1] = Vector2f(position.x() + offset.x(), position.y() - offset.y());   // Top-right
        vertices[2] = Vector2f(position.x() + offset.x(), position.y() + offset.y());   // Bottom-right
        vertices[3] = Vector2f(position.x() - offset.x(), position.y() + offset.y());   // Bottom-left
    }

    void update_frame(Uint16 now) {
        if ((now - last_frame_time) >= (1000.0f / sprite.fps)) {
            image_index = (image_index + 1) % sprite.frame_count;
            last_frame_time = now;
        }
    }

    void submit_vertices(Camera& cam) {
        apply_transform();
        render_batch_entity(*this, cam);
    }

private:
    // Transforms the current vertices with the entity's rotation value
    void apply_transform() {

        Vector2f size = Vector2f{scale.x() * sprite.frame_size.x(), scale.y() * sprite.frame_size.y()};
        Vector2f p_offset = get_pivot_offset(pivot, size);

        std::array<Vector2f, 4> center_p = {
            Vector2f{0, 0},         // TL
            Vector2f{size.x(), 0},  // TR
            size,                   // BR
            Vector2f{0, size.y()}   // BL
        };
        
        Affine2f matx = Affine2f::Identity();
        matx.translate(position);
        matx.rotate(deg_to_rad(rotation));      // In Degrees
        matx.scale(scale);                      // ermm.... ts pmo smh
        
        // Apply pivot offset
        matx.translate(-p_offset);

        // Apply to each vertex
        for (int i = 0; i < 4; ++i) {
            transformed_vertices[i] = matx * center_p[i];
        }
    }
};


int entity_spawn(std::string sprite, Vector2f pos, Vector2f scale, float rotation, Uint16 depth);
int entity_spawn(std::string sprite, Vector2f pos, Vector2f scale, float rotation, Pivot_Type pivot, Uint16 depth);
void entity_destroy(Uint16 id);
Entity& entity_get(Uint16 id);
int entity_count();

std::unordered_map<Uint16, Entity>& entity_get_map();
#endif