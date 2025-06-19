#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <Eigen/Dense>
using namespace Eigen;

/**
 * @brief Enum representing different pivot types for positioning.
 */
enum Pivot_Type {
    TOP_LEFT, TOP_CENTER, TOP_RIGHT,
    MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT,
    BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT
};

/**
 * @brief Returns the pivot offset for a given pivot type and size.
 * @param pivot_type The pivot type.
 * @param size The size (width, height) to calculate the offset for.
 * @return The offset as a Vector2f.
 */
Vector2f get_pivot_offset(Pivot_Type pivot_type, const Vector2f& size);


/**
 * @brief Generates a sprite sheet from individual frames.
 *
 * @param dir_p Absolute path to the directory containing all sprite frame images.
 *            Each frame should be named based on its index in the animation timeline,
 *            e.g., "0.png", "1.png", ..., "5.png" for a 6-frame animation.
 *
 * @param output Absolute path to the output sprite sheet file.
 *            The output filename should follow the format:
 *              spr_<name>_<frame_count>.png
 *            For example: "spr_player_6.png" represents a sprite sheet with 6 frames.
 *
 * @note
 *   - Frames are arranged left to right in the final sheet.
 * 
 *   - All frames must have the same dimensions.
 */
void generate_sprite_sheet(const std::string dir_p, const std::string output);


/**
 * @brief Converts all characters in the input string to lowercase (in-place).
 * 
 * @param str The input string to be converted. The original string is modified.
 * 
 * @note
 *   - Time complexity: O(n), where 'n' is the length of the string.
 */
void str_toLower(const std::string& str);


// Use Polynomial rolling hash function, for this case
// Generally used for Sprite_ID Generation
// But can still be applied to anywhere
uint64_t hash_string(const std::string& str);


/**
 * @brief Extracts the frame count from a sprite sheet filename.
 * @param filename The filename to parse.
 * @return The number of frames in the sprite sheet.
 */
uint32_t extract_frame_count(const std::string& filename);


/**
 * @brief Extracts the sprite name from a sprite sheet filename --lowercase.
 * @param filename The filename to parse.
 * @return The sprite name as a string.
 */
std::string extract_sprite_name(const std::string& filename);


/**
 * @brief Converts degrees to radians.
 * @param deg Angle in degrees.
 * @return Angle in radians.
 */
float deg_to_rad(float deg);


/**
 * @brief Converts radians to degrees.
 * @param rad Angle in radians.
 * @return Angle in degrees.
 */
float rad_to_deg(float rad);


/**
 * @brief Returns the distance between two points.
 * @param a Starting point.
 * @param b Target point.
 * @return the distance in float.
 */
float distance(const Vector2f& a, const Vector2f& b);

#endif