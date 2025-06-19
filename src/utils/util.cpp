#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <regex>
#include <cmath>
#include "util.hpp"
#include <Eigen/Dense>
using namespace Eigen;

#define PI SDL_PI_F

float deg_to_rad(float deg) {
    return deg * (PI / 180.0f);
}

float rad_to_deg(float rad) {
    return rad * (180.0f / PI);
}

float distance(const Vector2f& a, const Vector2f& b) {
    return sqrtf(
        powf(b.x() - a.x(), 2) + powf(b.y() - a.y(), 2)
    );
}

Vector2f get_pivot_offset(Pivot_Type pivot_type, const Vector2f& size) {
    float x = 0.0f;
    float y = 0.0f;

    switch (pivot_type) {
        case TOP_LEFT:        x = 0.0f;     y = 0.0f; break;
        case TOP_CENTER:      x = 0.5f;     y = 0.0f; break;
        case TOP_RIGHT:       x = 1.0f;     y = 0.0f; break;
        case MIDDLE_LEFT:     x = 0.0f;     y = 0.5f; break;
        case MIDDLE_CENTER:   x = 0.5f;     y = 0.5f; break;
        case MIDDLE_RIGHT:    x = 1.0f;     y = 0.5f; break;
        case BOTTOM_LEFT:     x = 0.0f;     y = 1.0f; break;
        case BOTTOM_CENTER:   x = 0.5f;     y = 1.0f; break;
        case BOTTOM_RIGHT:    x = 1.0f;     y = 1.0f; break;
    }

    return Vector2f(x * size.x(), y * size.y());
}

void generate_sprite_sheet(const std::string dir_p, const std::string output) {
    // check for folder existance
    DIR* dir = opendir(dir_p.c_str());
    if (dir == nullptr) {
        SDL_Log("Directory not Found. {%s}", dir_p.c_str());
        return;
    }

    // Folder walkthrough
    int i = 0;
    dirent* entity = readdir(dir);
    std::vector<SDL_Surface*> frames;

    // Get every frame as a surface
    while(entity != nullptr) {

        std::string img_path = (std::string(dir_p) + "/" + entity->d_name);
        SDL_Surface* frame = IMG_Load(img_path.c_str());
        if (frame == nullptr) {
            i++;
            entity = readdir(dir);
            continue;
        }

        frames.push_back(frame);
        entity = readdir(dir);
        i++;
    }

    // Validation
    if (frames.empty()) {
        SDL_Log("No Frames found. {%s}", dir_p.c_str());
        closedir(dir);
    }

    // Create a surface with the appropriate dimensions
    int frame_w = frames[0]->w;
    int frame_h = frames[0]->h;
    int total_w = frame_w * frames.size();

    SDL_Surface* sprite_sheet_surf = SDL_CreateSurface(
        total_w, frame_h, SDL_PIXELFORMAT_RGBA32
    );

    // Copy each frame to the sprite sheet surface
    for (int i = 0; i < frames.size(); i++) {
        SDL_Rect dest = {i * frame_w, 0, frame_w, frame_h};
        SDL_BlitSurface(frames[i], nullptr, sprite_sheet_surf, &dest);
        SDL_DestroySurface(frames[i]);
    }
    frames.clear();
    
    // Save as a Spritesheet file.
    IMG_SavePNG(sprite_sheet_surf, output.c_str());
    SDL_DestroySurface(sprite_sheet_surf);
    SDL_Log("> UTIL s> Sprite sheet generated. {%s}", output.c_str());
}

void str_toLower(std::string& str) {
    for (auto& x : str) {x = tolower(x);}
}

uint64_t hash_string(const std::string& str) {
    // p is a prime number
    // m is a large prime number
    long p = 31, m = (long) 1e9 + 9;

    long hval = 0;
    long pPow = 1;

    for (int i = 0; i < str.length(); i++) {
        hval = (hval + (str[i] - 'a' + 1)) * pPow % m;
        pPow = (pPow * p) % m;
    }
    return hval;
}

uint32_t extract_frame_count(const std::string& filename) {
    std::regex frame_regex("spr_.*_(\\d+)\\.png$");
    std::smatch match;
    if (std::regex_match(filename, match, frame_regex)) {
        return std::stoi(match[1]);
    }
    return -1; // Unknown
}

// File name Format: spr_<name>_<frame_Count>.png
std::string extract_sprite_name(const std::string& filename) {
    // Check that it starts with "spr_"
    if (filename.rfind("spr_", 0) != 0)
        return filename;

    // Remove prefix and suffix
    size_t start = 4; // after "spr_"
    size_t end = filename.find_last_of('_');

    if (end == std::string::npos || end <= start)
        return filename;

    std::string res = filename.substr(start, end - start);\
    str_toLower(res);
    return res;
}
