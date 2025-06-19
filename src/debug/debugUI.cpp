#include "debugUI.hpp"
#include "../engine/entity.hpp"
#include "../utils/util.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>
#include <string>

static bool show_stat_window = false;
static Entity* selected_entity;

void gui_draw_ready(UIManager& manager) {
    // ImGui Rendering
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void show_stats(char dbg_stats[][64], int stat_cnt) {
    if (!show_stat_window) {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(0, 24));
    ImGui::Begin("Status", nullptr,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings);
    for (int i = 0; i < stat_cnt; i++) {
        ImGui::Text("%s", dbg_stats[i]);
    }
    if (selected_entity != nullptr) {
        ImGui::Text("Selected Entity: %d", selected_entity->id);
    }
    ImGui::End();
}


void show_selected_ent() {
    if (selected_entity == nullptr) {
        return;
    }
    ImGui::Begin("Entity");
    ImGui::Text("ID: %d", selected_entity->id);
    ImGui::Text("Sprite: %s", selected_entity->sprite.sprite_name.c_str());
    ImGui::Text("Image Index: %d", selected_entity->image_index);
    ImGui::Text("Color Blend: %.2f, %.2f, %.2f, %.2f", 
        selected_entity->c_blend.r,
        selected_entity->c_blend.g,
        selected_entity->c_blend.b,
        selected_entity->c_blend.a
    );
    ImGui::Separator();
    ImGui::Text("Transform");
    ImGui::Text("Pivot Point: %s", get_pivot_name(selected_entity->pivot));
    ImGui::Text("Position: %.2f, %.2f", selected_entity->position.x(), selected_entity->position.y());
    ImGui::Text("Rotation: %.2f", selected_entity->rotation);
    ImGui::Text("Scale: %.2f, %.2f", selected_entity->scale.x(), selected_entity->scale.y());
    ImGui::End();
}

void debug_header(char dbg_stats[][64], int stat_cnt) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(5, 5));

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 23));
    ImGui::Begin("TopBar", nullptr,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Status", ImVec2(100, 23))) show_stat_window = !show_stat_window;
    ImGui::SameLine();
    ImGui::Button("Entity", ImVec2(100, 23));
    ImGui::SameLine();
    ImGui::Button("Settings", ImVec2(100, 23));
    ImGui::End();
    ImGui::PopStyleVar(4);
    show_stats(dbg_stats, stat_cnt);
    show_selected_ent();
}


void debug_entity(Entity* entity) {
    selected_entity = entity;
}

Entity& debug_selected_ent() {
    return *selected_entity;
}