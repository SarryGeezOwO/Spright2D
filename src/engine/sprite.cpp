#include "sprite.hpp"
#include "camera.hpp"
#include "../utils/util.hpp"
#include <SDL3/SDL.h>
#include <Eigen/Dense>
#include <SDL3_image/SDL_image.h>
#include <dirent.h>
#include <vector>
#include <string>
using namespace Eigen;

#define Skyline Vector2i
#define quad 4
#define SPRITE_DIR "assets/sprites/"
#define MAX_VERTEX_COUNT UINT16_MAX
#define MAX_ATLAS_SIZE 4096
#define ATLAS_PADDING 2

static SDL_Renderer* renderer = nullptr;
static std::vector<Skyline> skylines;
static Uint16 farthest_x;                   // Refers to the final width for Surface_atlas
static Uint16 farthest_y;                   // Refers to the final height for Surface_atlas

struct Sprite_buffer {
    SDL_Vertex  vertices[MAX_VERTEX_COUNT];
    int         indices[MAX_VERTEX_COUNT / 2 * 6];
    Uint16      vert_count;
    Uint16      index_count;
};

// DirName, SpriteSheetData
static std::unordered_map<std::string, Sprite_sheet_data> sprite_sheet_map;
static SDL_Surface* surface_atlas = nullptr; // Gets cleanup when texture_atlas is created
static SDL_Texture* texture_atlas = nullptr;

static std::unordered_map<std::string, Sprite_buffer> sprite_batch;  // SpriteID, Sprite_buffer
static int prev_rend_c = 0;
static int rendered_count = 0;

void load_all_sprite();
void sprite_add(std::string sprite_id, int fps);
int guess_fps(int frame_count) {
    if (frame_count <= 1) return 0;
    if (frame_count <= 4) return 6;
    if (frame_count <= 8) return 10;
    if (frame_count <= 12) return 12;
    if (frame_count <= 20) return 16;
    return 24;
}

// Next time friend
uint32_t hash_sprite_id(const std::string& name) {
    // FNV-1a (or any fast hash function)
    uint32_t hash = 2166136261u;
    for (char c : name) {
        hash ^= c;
        hash *= 16777619;
    }
    return hash;
}


SDL_Surface* crop_surface(SDL_Surface* src, int w, int h) {
    SDL_Surface* cropped = SDL_CreateSurface(w, h, src->format);

    SDL_Rect src_rect = { 0, 0, w, h };
    SDL_BlitSurface(src, &src_rect, cropped, nullptr);
    return cropped;
}


SDL_FRect frame_at(std::string sprite_id, Uint16 index) {
    Sprite_sheet_data& data = sprite_get(sprite_id);

    if (data.frame_count == 0 || index >= data.frame_count) {
        SDL_Log("Warning: Invalid frame index %d for sprite %s", index, sprite_id.c_str());
        return SDL_FRect{0, 0, 0, 0};
    }

    SDL_FRect rect;
    rect.x = data.location.x() + data.frame_size.x() * index;
    rect.y = data.location.y();
    rect.w = data.frame_size.x();
    rect.h = data.frame_size.y();
    return rect;
}


Vector4f frame_at_uv(std::string sprite_id, Uint16 index) {
    Sprite_sheet_data& data = sprite_get(sprite_id);

    if (data.frame_count == 0 || index >= data.frame_count) {
        SDL_Log("Warning: Invalid frame index %d for sprite %s", index, sprite_id.c_str());
        return Vector4f{0, 0, 0, 0};
    }

    Vector4f s_uv = data.UV_coord;
    float f_w = (s_uv.z() - s_uv.x()) / data.frame_count;

    float u_start = s_uv.x() + index * f_w;
    float u_end = u_start + f_w;

    Vector4f uv;
    uv.x() = u_start;
    uv.y() = s_uv.y();
    uv.z() = u_end;
    uv.w() = s_uv.w();
    return uv;
}




// ===============================================

void reset() {
    if (!surface_atlas) surface_atlas = SDL_CreateSurface(MAX_ATLAS_SIZE, MAX_ATLAS_SIZE, SDL_PIXELFORMAT_RGBA32);
    if (texture_atlas) SDL_DestroyTexture(texture_atlas);
    texture_atlas = nullptr; 
    sprite_sheet_map.clear();
    farthest_x = 0;
    farthest_y = 0;

    // Reset Skyline
    skylines.clear();
    skylines.push_back({0, 0}); // Flat ground
}

// A function for abstracting the adding process of a SpriteSheet to a texture atlas / SpriteSheet map
bool pack_sprite_sheet(const Vector2i size, Vector2i& out) {
    if (size.x() == 0 || size.y() == 0) 
        return false;
    
    Uint16 skyline_count = skylines.size();
    Uint16 max_w = MAX_ATLAS_SIZE;
    Uint16 max_h = MAX_ATLAS_SIZE;
    Uint16 w = size.x();
    Uint16 h = size.y();

    // Best candiate attributes
    Uint16 best_idx     = UINT16_MAX;
    Uint16 best_idx2    = UINT16_MAX;
    Uint16 bestX        = UINT16_MAX;
    Uint16 bestY        = UINT16_MAX;
    
    // For each skylines
    for (int i = 0; i < skyline_count; i++) {
        int x = skylines[i].x();
        int y = skylines[i].y();

        // If the current Skyline.x + requested Width Exceeds MAX_ATLAS_WIDTH, move on
        if (x + w > max_w) break;
        // We want the lowest Y possible
        if (y >= bestY) continue;

        // Raise Y to avoid overlaping skylines
        Uint16 xMax = x + w; 
        Uint16 i2;
        
        // Loop through all upcoming skylines relative to this current skyline point
        for (i2 = i + 1; i2 < skyline_count; i2++) {        // stops at The index where the spriteSheet stops overlapping
            if (xMax <= skylines[i2].x()) break;            // Check to see if Bottom Right and other ahead point is overlapping
            if (y < skylines[i2].y()) y = skylines[i2].y(); // We are looking for the Peakest Overlapping Skyline
        }

        // Don't get higher than the bestY or Max height(Atlas)
        if (y >= bestY || h > max_h)
            continue;

        best_idx = i;
        best_idx2 = i2;
        bestX = x;
        bestY = y;
    }

    // No Space to add
    if (best_idx == UINT16_MAX) return false;

    // Update Skyline silhouette
    Vector2i TL, BR;
    TL.x() = bestX;
    TL.y() = bestY + h;
    BR.x() = bestX + w;
    BR.y() = skylines[best_idx2 - 1].y();
    bool bBottomRight = (best_idx2 < skyline_count ? BR.x() < skylines[best_idx2].x() : BR.x() < max_w);

    // Clean up overlapped skylines
    if (best_idx2 > best_idx + 1)
        skylines.erase(skylines.begin() + best_idx + 1, skylines.begin() + best_idx2);
    
    skylines[best_idx] = TL;
    if (bBottomRight)
        skylines.insert(skylines.begin() + best_idx + 1, BR);

    if (farthest_x < BR.x()) farthest_x = BR.x();
    if (farthest_y < TL.y()) farthest_y = TL.y();

    out.x() = bestX;
    out.y() = bestY;
    return true;
}


void init_sprite_manager(SDL_Renderer* rend, bool load_all) {
    renderer = rend;
    reset();

    if (load_all) load_all_sprite(); 
}


void update_texture_atlas() {
    surface_atlas = crop_surface(surface_atlas, farthest_x, farthest_y);
    texture_atlas = SDL_CreateTextureFromSurface(renderer, surface_atlas);

    // TL = { x / a_w            y / a_h };
    // BR = { (x + w) / a_w      (y + h) / a_h  };

    for (auto& [key, value] : sprite_sheet_map) {
        Vector4f& uv = value.UV_coord;
        Vector2i& pos = value.location;
        Vector2i size = value.sheet_size();

        // Min UV
        uv.x() = (float)(pos.x() / (float)farthest_x);
        uv.y() = (float)(pos.y() / (float)farthest_y);

        // Max UV
        uv.z() = (float)(pos.x() + size.x()) / (float)farthest_x;
        uv.w() = (float)(pos.y() + size.y()) / (float)farthest_y;
    }

    IMG_SavePNG(surface_atlas, "Texture_atlas.png");
    SDL_DestroySurface(surface_atlas);
}


void load_all_sprite() {
    // Load Every sprite asset found
    DIR* dir = opendir(SPRITE_DIR);
    if (dir == nullptr) {
        SDL_Log("Directory not Found. {%s}", SPRITE_DIR);
        return;
    }

    // Folder walkthrough and get every frame as a surface
    dirent* entity = readdir(dir);
    while(entity != nullptr) {

        // validate Sprite file
        std::string img_path = (std::string(SPRITE_DIR) + entity->d_name);
        if (img_path.find("spr_") != std::string::npos) {

            std::string file_name = std::string(entity->d_name);
            SDL_Log("Adding Sprite Sheet {%s}", file_name.c_str());
            sprite_add(file_name, guess_fps(extract_frame_count(file_name)));

        }
        entity = readdir(dir);

    }

    update_texture_atlas();
}


// Expects a Sprite_sheet
// Sprite_id is the file_name
// Adds this sprite_sheet to the Texture atlas
//     example sprite_id: spr_player_5.png
// Automatically assigns position for each individual sprite_sheet
void sprite_add(std::string sprite_id, int fps) {
    
    std::string actual_id = extract_sprite_name(sprite_id);
    //uint32_t hash_id = hash_sprite_id(extract_sprite_name(sprite_id));
    
    // Check if sprite is already loaded
    auto it = sprite_sheet_map.find(sprite_id);
    if (it != sprite_sheet_map.end()) {
        SDL_Log("Sprite {%s} already exists.", sprite_id.c_str());
        return;
    }

    // Load Image file
    SDL_Surface* sprite_sheet =  IMG_Load((std::string(SPRITE_DIR) + sprite_id).c_str());
    if (!sprite_sheet) {
        SDL_Log("Failed to load sprite sheet: {%s}", sprite_id.c_str());
        return;
    }

    Sprite_sheet_data data = {};
    data.sprite_id = actual_id;
    data.fps = fps;
    data.loop = true;
    data.frame_count = extract_frame_count(sprite_id);

    Uint16 ss_width = sprite_sheet->w;
    Uint16 ss_height = sprite_sheet->h;
    data.frame_size = { ss_width / data.frame_count, ss_height };

    pack_sprite_sheet({ss_width, ss_height }, data.location);
    sprite_sheet_map[actual_id] = data;

    // Blit that surface to the Texture_Atlas
    SDL_Rect dest = {
        data.location.x(),
        data.location.y(),
        ss_width,
        ss_height
    };

    SDL_BlitSurface(sprite_sheet, nullptr, surface_atlas, &dest);
    SDL_DestroySurface(sprite_sheet);
    SDL_Log("  > Sprite Loaded Succesfully. {%s}", data.sprite_id.c_str());
    return;
}


void draw_sprite(std::string sprite_id, Uint8 index, float rotation, 
    Camera const cam, SDL_FRect rect) {

    Vector2f r = world_to_screen(cam, {rect.x, rect.y});
    rect.x = r.x();
    rect.y = r.y();

    Sprite_sheet_data data = sprite_get(sprite_id);
    SDL_FRect src = frame_at(sprite_id, index);

    SDL_RenderTextureRotated(
        renderer,
        texture_atlas,
        &src, &rect,
        rotation,
        NULL,
        SDL_FLIP_NONE  
    );
}


void draw_sprite_raw(std::string sprite_id, Uint8 index, float rotation, SDL_FRect rect) {
    Sprite_sheet_data data = sprite_get(sprite_id);
    SDL_FRect src = frame_at(sprite_id, index);

    SDL_RenderTextureRotated(
        renderer,
        texture_atlas,
        &src, &rect,
        rotation,
        NULL,
        SDL_FLIP_NONE  
    );
}


// Insert a sprite to be drawn in a depth batch
void batch_draw_sprite(std::string sprite_id, Uint8 index, float rotation, 
    Uint16 depth, std::array<Vector2f, 4>& vertices, Camera const cam) {
        
    // Should Draw?
    int v_out = 0;
    for (int i = 0; i < vertices.size(); i++) {
        if (camera_is_position_out(cam, vertices[i])) v_out++;
    }

    // If all vertex is out of camera view, then skip this sprite
    if (v_out >= 4) return;

    SDL_Vertex v0, v1, v2, v3;
    Vector2f tl = world_to_screen(cam, vertices[0]);
    Vector2f tr = world_to_screen(cam, vertices[1]);
    Vector2f br = world_to_screen(cam, vertices[2]);
    Vector2f bl = world_to_screen(cam, vertices[3]);
    Vector4f uv = frame_at_uv(sprite_id, index);

    // Top left
    v0.position.x = tl.x();
    v0.position.y = tl.y();
    v0.tex_coord.x = uv.x();
    v0.tex_coord.y = uv.y();
    v0.color = {1, 0, 1, 1}; 

    // Top Right
    v1.position.x = tr.x();
    v1.position.y = tr.y();
    v1.tex_coord.x = uv.z();
    v1.tex_coord.y = uv.y();
    v1.color = {1, 1, 0, 1};

    // Bottom right
    v2.position.x = br.x();
    v2.position.y = br.y();
    v2.tex_coord.x = uv.z();
    v2.tex_coord.y = uv.w();
    v2.color = {0, 1, 1, 1};
    
    // Bottom left
    v3.position.x = bl.x();
    v3.position.y = bl.y();
    v3.tex_coord.x = uv.x();
    v3.tex_coord.y = uv.w();
    v3.color = {1, 1, 1, 1};

    Sprite_buffer& buf = sprite_batch[sprite_id];
    Uint16& c   = buf.vert_count;
    Uint16& i   = buf.index_count;

    buf.indices[i++] = c + 0;
    buf.indices[i++] = c + 1;
    buf.indices[i++] = c + 2;
    buf.indices[i++] = c + 2;
    buf.indices[i++] = c + 3;
    buf.indices[i++] = c + 0;

    buf.vertices[c++] = v0;
    buf.vertices[c++] = v1;
    buf.vertices[c++] = v2;
    buf.vertices[c++] = v3;
    rendered_count++;
}


// Specifically draws all sprite loaded from the batch from a certain Depth value
void sprite_batch_draw_depth(Uint16 const depth) {

}


// Draws all sprite loaded from the batch with all Depth value
void sprite_batch_draw_all() {

    // For each Sprite_id request
    for (auto& [key, value] : sprite_batch) {

        // Render using the texture corresponding to this depth batch
        bool f = SDL_RenderGeometry(
            renderer, 
            texture_atlas, 
            value.vertices, 
            value.vert_count, 
            value.indices, 
            value.index_count
        );

        if (!f) {
            SDL_Log("Vert_count: %d", value.vert_count);
            SDL_Log("Idx_count: %d", value.index_count);
            SDL_Log("Error: {%s}", SDL_GetError());
        }

        /*
        if (prev_rend_c != rendered_count) {
            SDL_Log("Vertex count {%s}: %d", key.c_str(), value.vert_count);
        }
        */
    }

    if (prev_rend_c != rendered_count) {
        SDL_Log("Rendered: %d", rendered_count);
    }
}

void sprite_batch_clear() {
    sprite_batch.clear();
    prev_rend_c = rendered_count;
    rendered_count = 0;
}


void sprite_cleanup() {
    SDL_DestroyTexture(texture_atlas);
    SDL_DestroySurface(surface_atlas);
}


Sprite_sheet_data& sprite_get(std::string sprite_id) {
    return sprite_sheet_map.at(sprite_id);
}


int sprite_count() {
    return sprite_sheet_map.size();
}