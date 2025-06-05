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
Vector2f world_to_screen(Camera const cam, Vector2f const world_position);


/**
 * @brief Transforms a screen position to world coordinates (in-place).
 * @param cam The camera to use for transformation.
 * @param screen_position The screen position to transform (modified in-place).
 */
void screen_to_world_ref(Camera const cam, Vector2f& screen_position);


/**
 * @brief Transforms a screen position to world coordinates.
 * @param cam The camera to use for transformation.
 * @param screen_position The screen position to transform.
 * @return The transformed world position.
 */
Vector2f screen_to_world(Camera const cam, Vector2f const screen_position);


/**
 * @brief Returns a reference to the culling margin value for all cameras.
 * @return Reference to the integer culling margin.
 */
int& cam_culling_margin();


/**
 * @brief Checks if a world position is outside the camera's bounding view.
 * @param cam The camera to check against.
 * @param world_position The world position to test.
 * @return True if the position is outside the camera's view, false otherwise.
 */
bool camera_is_position_out(Camera const cam, Vector2f const world_position);

#endif