/*

AUTHOR: SarryGeezOWO (https://github.com/SarryGeezOwO)
DATE: June / 19 / 2025

This is my first ever big 2D-game engine project, This application is not the best
in terms of code readability or optimizations, as well as the features. This is
solely made for my educational purposes as this type of topic is rarely discussed 
on schools. (Huge Skill issue ngl 😞)

*/

#include <iostream>
#include <string>
#include <bitset>
#include <chrono>
#include <filesystem>

// External Libraries ========
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

#include <Eigen/Dense>
using namespace Eigen;

// User define ========
#include "debug/debugUI.hpp"
#include "engine/camera.hpp"
#include "engine/renderer.hpp"
#include "engine/sprite.hpp"
#include "engine/entity.hpp"
#include "core/input.hpp"
#include "utils/util.hpp"

// Constants ========
#define FPS 60
#define MS_PER_FRAME 1000 / FPS
#define WIN_TITLE "Spright2D v.1.0"
#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080
#define STAT_COUNT 7

// Struct for handling state for each scene
struct global_state {
    /* States */
};

struct local_state {
    /* States */
};

// Add Global variables, this variables are available to all scenes.
const int GRAVITY = 5;

// =========================== RESOURCES =====================================
SDL_Window* win;
SDL_Renderer* renderer;
SDL_Event event;
UIManager ui_manager;
char dbg_stats[STAT_COUNT][64]; // 64 bytes per line, tweak as needed

bool game_running = true;   // Stop or Continue Game loop
bool invokeStart = true;    // Resets every Scene Change, on True call start
float current_fps = 0.0f;   // Non-capped FPS
Uint32 delta_time;          // Measured in Milliseconds
Uint32 frame_count = 0;
Uint32 frame_timer = 0;
Camera camera = {
    {-WIN_WIDTH/2, -WIN_HEIGHT/2},
    {WIN_WIDTH, WIN_HEIGHT}
};

// Cache sprite IDs
Uint64 spr_player = hash_string("player");

// Function Declarations
void app_quit();
void config_sprite();
void load_entities();

// Initiate SDL3, Window, and Renderer
void init() {

    // Check current Directory
    std::string cd = std::string("CWD: ") + std::filesystem::current_path().string();
    SDL_Log(cd.c_str());

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed SDL3 Initialization.");
        app_quit();
    }

    win = SDL_CreateWindow(WIN_TITLE, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (win == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to create a window kekw.");
        app_quit();
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    renderer = SDL_CreateRenderer(win, NULL);
    if (renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create renderer.");
        app_quit();
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO &io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL3_InitForSDLRenderer(win, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Initialization of System (Peak shit🙏🙏)
    init_sprite_manager(renderer);   // Load all sprite_sheets
    config_sprite();
    render_init(renderer);
    flip_event(DEBUG_MODE);          // Initially start with debug mode

    //  >>>> You wanna Generate some sprite_sheets? Do it below. <<<<<<   
    // generate_sprite_sheet("dir", "spr_output_0.png");
}

// FPS and shits
void config_sprite() {
    sprite_get("enemy").fps     = 8;
    sprite_get("cat").fps       = 6;
    sprite_get("player").fps    = 6;
}

void load_entities() {
    // Base Scene
    entity_spawn("player", {150, 300}, {2, 2}, 0, MIDDLE_CENTER, 200);
}


// Scene start ====================================================================
void start() { 
    /* CODE */
}

// Runs per frame =====================================================================
int spwn_time = 0;
void update(global_state& gs, local_state& ls) {
    /* CODE */
    Vector2f m_w = screen_to_world(camera, mouse_pos());

    // Entity Clicking
    int ent_id = -1;
    for (auto& [key, ent] : entity_get_map()) {
        int w = ent.scale.x() * ent.sprite.frame_size.x();
        if (distance(m_w, ent.position) < w && is_event_active(MOUSE_RIGHT_PRESSED) && ent_id == -1) {
            ent_id = ent.id;
        }
        else {ent.c_blend = {1, 1, 1, 1};}
    }

    if (ent_id != -1) {
        Entity& e = entity_get(ent_id);
        debug_entity(&e);
    }

    Entity* dbg_ent = &debug_selected_ent();
    if (dbg_ent != nullptr) {
        dbg_ent->c_blend = {1, 1, 0, 1};
    }
    
    if (is_event_active(MOUSE_LEFT_PRESSED)) {
        Entity& e = entity_get(0);
        Vector2i h_size = e.sprite.frame_size / 2;
        e.position.x() = m_w.x();
        e.position.y() = m_w.y();
    }

    if (check_key(SDL_SCANCODE_X)) entity_get(0).rotation += 2; 
    if (check_key(SDL_SCANCODE_N)) entity_get(0).scale.x() += 0.1; 
    if (check_key(SDL_SCANCODE_M)) entity_get(0).scale.y() += 0.1; 
    if (check_key(SDL_SCANCODE_K)) entity_get(0).scale.x() -= 0.1; 
    if (check_key(SDL_SCANCODE_L)) entity_get(0).scale.y() -= 0.1; 

    float cam_spd = 5;
    if (check_key(SDL_SCANCODE_W)) camera.move({0       ,  -cam_spd });
    if (check_key(SDL_SCANCODE_A)) camera.move({-cam_spd,  0        });
    if (check_key(SDL_SCANCODE_S)) camera.move({0       ,  cam_spd  });
    if (check_key(SDL_SCANCODE_D)) camera.move({cam_spd ,  0        });
    
    // TEST Spawn on mouse position
    if (spwn_time > 0) {
        spwn_time -= delta_time;
        return;
    }

    std::string id = (check_key(SDL_SCANCODE_E) ? "enemy" : "none");
    id = (check_key(SDL_SCANCODE_Q) ? "cat" : id);

    if (id != "none") {
        entity_spawn(id, {m_w.x(), m_w.y()}, {1, 1}, 0, TOP_CENTER, 100);
        spwn_time = 30;
    }
}

// Renders Drawable Objects ===========================================================
void render(const global_state gs, const local_state ls) {
    /* CODE (Always white on start) */
    draw_sprite_raw(spr_player, 0, 45, {200, 200, 200, 200});
}

// Debug shit
void debug_update() {
    if (!is_event_active(DEBUG_MODE)) return;
    snprintf(dbg_stats[0], 64, "FPS: %.2f", current_fps);
    snprintf(dbg_stats[1], 64, "Sprite Loaded: %d", sprite_count());
    snprintf(dbg_stats[2], 64, "Entity Count: %d", entity_count());
    snprintf(dbg_stats[3], 64, "Rendered: %d", rendered_count());
    snprintf(dbg_stats[4], 64, "Spawn time: %d", spwn_time);

    const char* dm = is_event_active(DEBUG_MODE) ? "true" : "false";
    snprintf(dbg_stats[5], 64, "Debug Mode: %s", dm);
    snprintf(dbg_stats[6], 64, "Camera Pos: %.2f, %.2f", camera.x(), camera.y());
}



// ============================ MAIN =============================== //

int main(int argc, char* argv[]) {
    SDL_Log("Application starting...");
    init();

    global_state gs = {};
    local_state ls = {};

    Uint64 previous = SDL_GetTicks();
    Uint64 lag = 0.0;

    Uint64 before = SDL_GetTicks();
    load_entities();
    SDL_Log("Time to load: %d ms.", SDL_GetTicks() - before);

    // Game Loop
    while (game_running)
    {
        Uint64 frame_start = SDL_GetTicks();

        // Start event
        if (invokeStart) {
            start();
            invokeStart = false;
        }

        Uint64 current = SDL_GetTicks();
        delta_time = current - previous;
        previous = current;
        lag += delta_time;

        // Lag Compensation
        game_running = input_handle_event(&event);
        while (lag >= MS_PER_FRAME)
        {
            update(gs, ls);
            debug_update();
            lag -= MS_PER_FRAME; 
        }
 
        // Entities rendering
        render_batch_clear_all();
        for (auto& [key, value] : entity_get_map()) {
            value.update_frame(current);
            value.update_vertices();
            value.apply_transform();
            value.submit_vertices(camera);
        }

        // Rendering
        gui_draw_ready(ui_manager);
        debug_header(dbg_stats, STAT_COUNT);
        if (is_event_active(DEBUG_MODE)) {
            ui_manager.render();
        }
        ImGui::Render();

        // Render Reset
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Always reset back to white
        render(gs, ls);
        
        // Renders all vertex buffers with texture i.e, An Entity lol
        render_batch_all(true);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer); 

        // Frame time
        Uint64 frame_end = SDL_GetTicks();
        Uint64 frame_time = frame_end - frame_start;

        frame_count++;
        frame_timer += frame_time;
        if (frame_timer >= 1000) {
            current_fps = (frame_count * 1000 / frame_timer);
            frame_count = 0;
            frame_timer = 0;
        }

    }

    app_quit();
    return EXIT_SUCCESS;
}


// System CLean-up
void app_quit() {
    sprite_cleanup();
    if (win != nullptr) SDL_DestroyWindow(win);
    if (renderer != nullptr) SDL_DestroyRenderer(renderer);

    // ImGui
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui::DestroyContext();

    SDL_Log("Exiting program...");
    SDL_Quit();
}