#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Eigen/Dense"
using namespace Eigen;

struct Camera {
    Vector2f position;  /**< The top-left position of the camera in world coordinates. */
    Vector2f size;      /**< The size (width, height) of the camera viewport. */

    /**
     * @brief Gets the x-coordinate of the camera position.
     * @return The x value.
     */
    float x() {
        return position.x();
    }

    /**
     * @brief Gets the y-coordinate of the camera position.
     * @return The y value.
     */
    float y() {
        return position.y();
    }

    /**
     * @brief Moves the camera by a given delta.
     * @param delta The amount to move the camera.
     */
    void move(Vector2f const delta) {
        position += delta;
    }

    /**
     * @brief Gets the Top-left and Bottom-right of this camera
     * @return the Bbox in a Vector4f Format (x, y, z, w)
     */
    Vector4f bbox() const {
        return {
            position.x(), 
            position.y(),
            position.x() + size.x(), 
            position.y() + size.y()
        };
    }
};


/**
 * @brief Transforms a world position to screen coordinates (in-place).
 * @param cam The camera to use for transformation.
 * @param world_position The world position to transform (modified in-place).
 */
void world_to_screen_ref(Camera const cam, Vector2f &world_position);


/**
 * @brief Transforms a world position to screen coordinates.
 * @param cam The camera to use for transformation.
 * @param world_position The world position to transform.
 * @return The transformed screen position.
 */
Vector2f world_to_screen(const Camera& cam, Vector2f const world_position);


/**
 * @brief Transforms a screen position to world coordinates (in-place).
 * @param cam The camera to use for transformation.
 * @param screen_position The screen position to transform (modified in-place).
 */
void screen_to_world_ref(const Camera& cam, Vector2f& screen_position);


/**
 * @brief Transforms a screen position to world coordinates.
 * @param cam The camera to use for transformation.
 * @param screen_position The screen position to transform.
 * @return The transformed world position.
 */
Vector2f screen_to_world(const Camera& cam, Vector2f const screen_position);


/**
 * @brief Returns a reference to the culling margin value for all cameras.
 * @return Reference to the integer culling margin.
 */
float& cam_culling_margin();


/**
 * @brief Checks if a world position is inside the camera's bounding view.
 * @param cam The camera to check against.
 * @param world_position The world position to test.
 * @return True if the position is seen inside the camera view
 */
bool camera_is_position_in(const Camera& cam, Vector2f world_position);


/**
 * @brief Checks if a quad is inside the camera's bounding view
 */
bool camera_is_quad_in(const Camera& cam, Vector4f const quad_bbox);

#endif