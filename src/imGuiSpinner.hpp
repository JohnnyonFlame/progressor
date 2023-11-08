#pragma once
#include <vector>
#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <sstream>

namespace ImGui {
    void Spinner(float radius, int thickness, const ImU32& color)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;
        
        ImGuiContext& g = *GImGui;        
        ImVec2 pos = window->DC.CursorPos + window->Scroll;
        
        // Render
        window->DrawList->PathClear();
        
        int num_segments = 30;        
        float start = abs(ImSin(g.Time*1.8f)*(num_segments-5));
        
        const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI*2.0f * ((float)num_segments-3) / (float)num_segments;

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(pos.x + ImCos(a+g.Time*8) * radius,
                                                pos.y + ImSin(a+g.Time*8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);
    }
}