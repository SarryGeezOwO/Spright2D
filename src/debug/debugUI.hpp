#ifndef DEBUGUI_HPP
#define DEBUGUI_HPP

#include "../engine/entity.hpp"
#include <vector>
#include <functional>
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

/**
 * @struct UIFrame
 * @brief Represents a single UI window/frame in the debug UI system.
 *
 * Each UIFrame contains a title and a callback function that is called to render
 * the contents of the frame using ImGui.
 */
struct UIFrame {
private:
    std::function<void()> callback; ///< The function to call for rendering the frame's contents.
    std::string title;              ///< The title of the UI frame.

public :
    /**
     * @brief Constructs a UIFrame with a title and a render callback.
     * @param title The title of the frame.
     * @param callback_func The function to call for rendering the frame's contents.
     */
    UIFrame(const std::string& title, std::function<void()> callback_func) {
        this->title = title;
        this->callback = callback_func;
    }

    /**
     * @brief Renders the UI frame using ImGui.
     *
     * Calls the stored callback function within an ImGui window.
     */
    void render() {
        if (callback) {callback();}
    }
};


/**
 * @struct UIManager
 * @brief Manages multiple UIFrame instances and renders them.
 *
 * UIManager stores a collection of UIFrame objects and provides methods to add
 * new frames and render all frames.
 */
struct UIManager {
private:
    std::vector<UIFrame> frames; ///< Collection of UIFrame objects.

public:
    /**
     * @brief Adds a new UIFrame to the manager.
     * @param frame The UIFrame to add.
     */
    void add_frame(UIFrame frame) {
        frames.push_back(frame);
    }

    /**
     * @brief Renders all managed UIFrames.
     */
    void render() {
        for (auto& frame : frames) {
            frame.render();
        }
    }
};


/**
 * @brief Prepares and renders the debug UI using ImGui and the given UIManager.
 *
 * This function sets up a new ImGui frame, renders all UIFrames managed by the
 * provided UIManager, and finalizes the ImGui rendering.
 *
 * @param manager The UIManager containing all UIFrames to render.
 */
void gui_draw_ready(UIManager& manager);


void debug_header(char dbg_stats[][64], int stat_cnt);


void debug_entity(Entity* entity);


Entity& debug_selected_ent();
#endif