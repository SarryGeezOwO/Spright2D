#include "sprite.hpp"
#include "entity.hpp"
#include <map>
#include <string>
#include <Eigen/Dense>
#include <functional>
using namespace Eigen;

// Ent_ID, EntityData
static std::unordered_map<Uint16, Entity> entity_map;
static Uint32 id_cursor = 0;

// Return the assigned ID for this entity
int entity_spawn(std::string sprite, Vector2f pos, Vector2f scale, float rotation, Uint16 depth) {
    if (entity_map.size() >= MAX_ENTITIES) {
        return -1;
    }

    Entity ent = {};
    ent.id = id_cursor;
    ent.depth = depth;
    ent.position = pos;
    ent.rotation = rotation;
    ent.scale = scale;
    ent.sprite = sprite_get(sprite);
    ent.image_index = 0;
    entity_map[id_cursor] = ent; 

    return id_cursor++;
}

// Return the assigned ID for this entity
int entity_spawn(std::string sprite, Vector2f pos, Vector2f scale, float rotation, Pivot_Type pivot, Uint16 depth) {
    if (entity_map.size() >= MAX_ENTITIES) {
        return -1;
    }

    Entity ent = {};
    ent.pivot = pivot;
    ent.id = id_cursor;
    ent.depth = depth;
    ent.position = pos;
    ent.rotation = rotation;
    ent.scale = scale;
    ent.sprite = sprite_get(sprite);
    ent.image_index = 0;
    entity_map[id_cursor] = ent; 

    return id_cursor++;
}


void entity_destroy(Uint16 id) {
    entity_map.erase(id);
}


// Performance shit from here
/*
void entity_render_all(SDL_Renderer* rend, Camera &cam) {
    if (entity_map.empty()) return;

    for (auto& [key, value] : entity_map) {
        // Check if at least one vertex is inside the camera view
        bool is_visible = false;
        for (const auto& vertex : value.vertices()) {
            if (!camera_is_position_out(cam, vertex)) {
                is_visible = true;
                break;
            }
        }

        if (is_visible) {
            value.render(rend, cam);
        }
    }
}
*/


Entity& entity_get(Uint16 id) {
    return entity_map[id];
}


int entity_count() {
    return entity_map.size();
}

std::unordered_map<Uint16, Entity>& entity_get_map() {
    return entity_map;
}