#include <iostream>
#include <string>
#include <bitset>
#include <chrono>
#include <filesystem>

// External Libraries
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <Eigen/Dense>
using namespace Eigen;

// User define
#include "engine/camera.hpp"
#include "engine/renderer.hpp"
#include "engine/sprite.hpp"
#include "engine/entity.hpp"
#include "core/input.hpp"
#include "utils/util.hpp"

// Constants
#define FPS 60
#define MS_PER_FRAME 1000 / FPS
#define WIN_TITLE "Spright2D v.1.0"
#define WIN_WIDTH 1000
#define WIN_HEIGHT 720

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

// Helper Functions
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
    for (auto& [key, ent] : entity_get_map()) {
        int w = ent.scale.x() * ent.sprite.frame_size.x();
        if (distance(m_w, ent.position) < w) {
            ent.c_blend = {1, 1, 0, 1};
        }
        else {
            ent.c_blend = {1, 1, 1, 1};
        }
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
        entity_spawn(id, {m_w.x(), m_w.y()}, {1, 1}, 0, MIDDLE_CENTER, 100);
        spwn_time = 50;
    }
}

// Renders Drawable Objects ===========================================================
void render(const global_state gs, const local_state ls) {
    /* CODE (Always white on start) */
    draw_sprite_raw(spr_player, 0, 45, {200, 200, 200, 200});
}

// Debug shit
void debug_draw() {
    if (!is_event_active(DEBUG_MODE)) return;
    SDL_RenderDebugTextFormat(renderer, 20, 20, "FPS: %.2f", current_fps);
    SDL_RenderDebugTextFormat(renderer, 20, 40, "Sprite Loaded: %d", sprite_count());
    SDL_RenderDebugTextFormat(renderer, 20, 60, "Entity Count: %d", entity_count());
    SDL_RenderDebugTextFormat(renderer, 20, 80, "Rendered: %d", rendered_count());
    SDL_RenderDebugTextFormat(renderer, 20,100, "Spawn time: %d", spwn_time);

    // Vertical
    Vector2f vl_s = world_to_screen(camera, {0, -10});
    Vector2f vl_e = world_to_screen(camera, {0, 10});
    SDL_RenderLine(renderer, vl_s.x(), vl_s.y(), vl_e.x(), vl_e.y());

    // Horizontal
    Vector2f hl_s = world_to_screen(camera, {-10, 0});
    Vector2f hl_e = world_to_screen(camera, {10, 0});
    SDL_RenderLine(renderer, hl_s.x(), hl_s.y(), hl_e.x(), hl_e.y());
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
        game_running = input_handle_event();
        while (lag >= MS_PER_FRAME)
        {
            update(gs, ls);
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
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Always reset back to white

        debug_draw();
        render(gs, ls);
        
        // Renders all vertex buffers with texture i.e, An Entity lol
        render_batch_all(true);
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

    SDL_Log("Exiting program...");
    SDL_Quit();
}