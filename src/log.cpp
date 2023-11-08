#include <string>
#include <vector>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include "imGuiSpinner.hpp"
#include "imGuiAnsiColor.hpp"
#include "style.hpp"

void RenderMessages(const std::string &lines, bool spins)
{
    ImGuiContext& g = *GImGui;

    // Draw stuff
    ImGui::TextUnformatted(lines.c_str(), lines.c_str()+lines.size());

    // Auto-scroll
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    static float acc = 0;
    static float add = 0;
    bool down = ImGui::IsKeyDown(ImGuiKey_GamepadDpadDown, ImGuiKeyOwner_None);
    bool up = ImGui::IsKeyDown(ImGuiKey_GamepadDpadUp, ImGuiKeyOwner_None);
    if (down) add += (512.f * acc + 256.f) * g.IO.DeltaTime;
    if (up)   add -= (512.f * acc + 256.f) * g.IO.DeltaTime;
    if (down || up) {
        // You must always scroll on integer intervals, so accumulate errors
        // and scroll on integer steps.
        float integer;
        add = modff(add, &integer);
        ImGui::SetScrollY(ImGui::GetScrollY() + integer);
        acc += g.IO.DeltaTime;
    } else {
        acc = 0;
    }
    
    if (spins) {
        float where = (16.f + 32.f) * GetUIScale();
        ImGui::SetCursorPos(ImGui::GetWindowSize() - ImVec2(where, where));
        ImGui::Spinner(16 * GetUIScale(), 4 * GetUIScale(), ImGui::GetColorU32(g.Style.Colors[ImGuiCol_NavHighlight]));
    }
}
