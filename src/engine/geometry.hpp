#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include <Eigen/Dense>
using namespace Eigen;

/**
 * @brief Polygon with N vertices (fixed at compile time).
 */
template <size_t N>
struct Polygon {
    std::array<Vector2f, N> vertices;

    Polygon(const std::array<Vector2f, N>& verts) : vertices(verts) {}
    Polygon() {}

    Vector2f& operator[](size_t i) { 
        return vertices[i]; 
    }

    const Vector2f& operator[](size_t i) const {
        return vertices[i]; 
    }

    constexpr size_t size() const { 
        return N; 
    }
};

template <size_t N>
std::vector<int> triangulate(Polygon<N> points) {
    return {0, 0, 0};
}

#endif