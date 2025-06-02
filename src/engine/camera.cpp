#include "camera.hpp"
#include "Eigen/Dense"
using namespace Eigen;

static int culling_margin = 25;

int& cam_culling_margin() {
    return culling_margin;
}

void world_to_screen_ref(Camera const cam, Vector2f& world_position) {
    world_position - cam.position;
}

Vector2f world_to_screen(Camera const cam, Vector2f const world_position) {
    return world_position - cam.position;
}

void screen_to_world_ref(Camera const cam, Vector2f& screen_position) {
    screen_position + cam.position;
}

Vector2f screen_to_world(Camera const cam, Vector2f const screen_position) {
    return screen_position + cam.position;
}


// Return True is given position is out of camera view
bool camera_is_position_out(Camera const cam, Vector2f const world_position) {
    return !(
        world_position.x() >= cam.position.x() - culling_margin &&
        world_position.x() <= cam.position.x() + (cam.size.x() + culling_margin) &&
        world_position.y() >= cam.position.y() - culling_margin &&
        world_position.y() <= cam.position.y() + (cam.size.y() + culling_margin)
    );
}