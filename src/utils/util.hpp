#ifndef UTIL_HPP
#define UTIL_HPP

#include <SDL3/SDL.h>
#include <string>

// Generates a sprite sheet from individual frames.
//
// Parameters:
//   dir_p  - Absolute path to the directory containing all sprite frame images.
//            Each frame should be named based on its index in the animation timeline,
//            e.g., "0.png", "1.png", ..., "5.png" for a 6-frame animation.
//
//   output - Absolute path to the output sprite sheet file.
//            The output filename should follow the format:
//              spr_<name>_<frame_count>.png
//            For example: "spr_player_6.png" represents a sprite sheet with 6 frames.
//
// Notes:
//   - Frames are arranged left to right in the final sheet.
//   - All frames must have the same dimensions.
//
void generate_sprite_sheet(const std::string dir_p, const std::string output);

Uint32 extract_frame_count(const std::string& filename);

std::string extract_sprite_name(const std::string& filename);

#endif