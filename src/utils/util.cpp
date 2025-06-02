#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <regex>
#include "util.hpp"

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
    SDL_Log("> UTIL > Sprite sheet generated. {%s}", output.c_str());
}

Uint32 extract_frame_count(const std::string& filename) {
    std::regex frame_regex("spr_.*_(\\d+)\\.png$");
    std::smatch match;
    if (std::regex_match(filename, match, frame_regex)) {
        return std::stoi(match[1]);
    }
    return -1; // Unknown
}

std::string extract_sprite_name(const std::string& filename) {
    // Check that it starts with "spr_"
    if (filename.rfind("spr_", 0) != 0)
        return filename;

    // Remove prefix and suffix
    size_t start = 4; // after "spr_"
    size_t end = filename.find_last_of('_');

    if (end == std::string::npos || end <= start)
        return filename;

    return filename.substr(start, end - start);
}
