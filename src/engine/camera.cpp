#include "camera.hpp"
#include "geometry.hpp"
#include "Eigen/Dense"
using namespace Eigen;

static float culling_margin = 200;

float& cam_culling_margin() {
    return culling_margin;
}

void world_to_screen_ref(const Camera& cam, Vector2f& world_position) {
    world_position - cam.position;
}

Vector2f world_to_screen(const Camera& cam, Vector2f const world_position) {
    return world_position - cam.position;
}

void screen_to_world_ref(const Camera& cam, Vector2f& screen_position) {
    screen_position + cam.position;
}

Vector2f screen_to_world(const Camera& cam, Vector2f const screen_position) {
    return screen_position + cam.position;
}

// Return True is given position is seen inside the camera view
bool camera_is_position_in(const Camera& cam, const Vector2f& world_position) {
    Vector4f bbox = cam.bbox();
    bbox.x() -= culling_margin;
    bbox.y() -= culling_margin;
    bbox.z() += culling_margin;
    bbox.w() += culling_margin;

    return (
        world_position.x() >= bbox.x() && world_position.x() <= bbox.z() &&
        world_position.y() >= bbox.y() && world_position.y() <= bbox.w()
    );
}