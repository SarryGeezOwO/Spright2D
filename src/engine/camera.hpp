#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Eigen/Dense"
using namespace Eigen;

struct Camera {
    Vector2f position;
    Vector2f size;

    float x() {
        return position.x();
    }

    float y() {
        return position.y();
    }

    void move(Vector2f const delta) {
        position += delta;
    }
};

// Transform a world position relative to the camera
// modifies the position relative to the camera
void world_to_screen_ref(Camera const cam, Vector2f &world_position);
Vector2f world_to_screen(Camera const cam, Vector2f const world_position);

void screen_to_world_ref(Camera const cam, Vector2f& screen_position);
Vector2f screen_to_world(Camera const cam, Vector2f const screen_position);

// Returns a read and write int value representing the culling margin of every camera
int& cam_culling_margin();


// Checks if the given position(World_position) is outside of the camera's bounding view
bool camera_is_position_out(Camera const cam, Vector2f const world_position);
#endif