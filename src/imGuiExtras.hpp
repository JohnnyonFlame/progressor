#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include "jet.hpp"

namespace ImGui {
    void AlignForWidth(float width, float alignment = 0.5f)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        float avail = ImGui::GetContentRegionAvail().x;
        float off = (avail - width) * alignment;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }

    template <class ...Args>
    void CenterButtons(Args &&...args)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        int width = ((ImGui::CalcTextSize(args).x + style.ItemSpacing.x) + ...);
        AlignForWidth(width + style.ItemSpacing.x);
    }
};