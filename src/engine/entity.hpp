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

/**
 * @brief Represents a game entity with transform, sprite, and rendering data.
 * 
 * Stores all necessary information for rendering and animating an entity,
 * including position, scale, rotation, sprite data, and vertex arrays.
 */
struct Entity {
private:
    Affine2f matx = Affine2f::Identity();

public:
    int id;                          /**< The index location in the entity array. */
    Pivot_Type pivot = TOP_LEFT;     /**< The point where position rests, defaults to TOP_LEFT. */
    Sprite_sheet_data sprite;        /**< The sprite sheet to refer to. */
    Uint8 image_index;               /**< The current frame of the sprite. */
    Uint64 last_frame_time;          /**< Tracking time for FPS. */
    std::array<Vector2f, 4> vertices;               /**< Original points of this entity (no rotation/scale). */
    std::array<Vector2f, 4> transformed_vertices;   /**< Vertices with applied rotation and scale. */

    // Transform
    Uint16 depth;                    /**< Entity depth draw, defaults to 100. */
    Vector2f position;               /**< World position. */
    Vector2f scale;                  /**< Scale factor. */
    float rotation;                  /**< Rotation in degrees. */


    /**
     * @brief Get's the Bounding box of this entity, (transformed)
     * @return The Top-left and Bottom-right of this entity [x, y, z, w]
     */
    Vector4f bbox() const {
        return {
            transformed_vertices[0].x(),
            transformed_vertices[0].y(),
            transformed_vertices[2].x(),
            transformed_vertices[2].y()  
        };   
    }

    
    /**
     * @brief Updates the entity's vertex positions based on position, and pivot.
     */
    void update_vertices() {
        Vector2f size = Vector2f{sprite.frame_size.x(), sprite.frame_size.y()};
        Vector2f offset = get_pivot_offset(pivot, size);
        vertices[0] = Vector2f(position.x() - offset.x(), position.y() - offset.y());   // Top-left
        vertices[1] = Vector2f(position.x() + offset.x(), position.y() - offset.y());   // Top-right
        vertices[2] = Vector2f(position.x() + offset.x(), position.y() + offset.y());   // Bottom-right
        vertices[3] = Vector2f(position.x() - offset.x(), position.y() + offset.y());   // Bottom-left
    }


    /**
     * @brief Updates the animation frame based on elapsed time.
     * @param now The current time in milliseconds.
     */
    void update_frame(Uint16 now) {
        if ((now - last_frame_time) >= (1000.0f / sprite.fps)) {
            image_index = (image_index + 1) % sprite.frame_count;
            last_frame_time = now;
        }
    }

    /**
     * @brief Applies transformation and submits the entity's vertices for rendering.
     * @param cam Reference to the camera for rendering transformations.
     */
    void submit_vertices(const Camera& cam) {
        render_batch_entity(*this, cam);
    }

    /**
     * @brief Applies rotation, scale, and pivot offset to the entity's vertices.
     */
    void apply_transform() {

        Vector2f size = Vector2f{scale.x() * sprite.frame_size.x(), scale.y() * sprite.frame_size.y()};
        Vector2f p_offset = get_pivot_offset(pivot, size);
        matx = Affine2f::Identity();

        std::array<Vector2f, 4> center_p = {
            Vector2f{0, 0},         // TL
            Vector2f{size.x(), 0},  // TR
            size,                   // BR
            Vector2f{0, size.y()}   // BL
        };
        
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


/**
 * @brief Spawns a new entity with the given parameters.
 * 
 * @param sprite The sprite ID to use.
 * @param pos The world position.
 * @param scale The scale factor.
 * @param rotation The rotation in degrees.
 * @param depth The rendering depth.
 * @return The ID of the spawned entity.
 */
int entity_spawn(std::string sprite, Vector2f pos, Vector2f scale, float rotation, Uint16 depth);


/**
 * @brief Spawns a new entity with the given parameters and pivot type.
 * 
 * @param sprite The sprite ID to use.
 * @param pos The world position.
 * @param scale The scale factor.
 * @param rotation The rotation in degrees.
 * @param pivot The pivot type.
 * @param depth The rendering depth.
 * @return The ID of the spawned entity.
 */
int entity_spawn(std::string sprite, Vector2f pos, Vector2f scale, float rotation, Pivot_Type pivot, Uint16 depth);


/**
 * @brief Destroys the entity with the given ID.
 * @param id The ID of the entity to destroy.
 */
void entity_destroy(Uint16 id);


/**
 * @brief Retrieves a reference to the entity with the given ID.
 * @param id The ID of the entity.
 * @return Reference to the Entity.
 */
Entity& entity_get(Uint16 id);


/**
 * @brief Returns the total number of active entities.
 * @return The number of entities.
 */
int entity_count();


/**
 * @brief Returns a reference to the map of all entities.
 * @return Reference to the unordered_map of entities.
 */
std::unordered_map<Uint16, Entity>& entity_get_map();
#endif