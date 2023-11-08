#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include "jet.hpp"

namespace ImGui {
    void TextCentered(const std::string &text)
    {
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        float max_text_width = CalcTextSize(text.c_str()).x;

        for (const auto &line: jet::split(text, "\n")) {
            float text_width = CalcTextSize(line.c_str()).x;
            SetCursorPosX((style.FramePadding.x + GetCursorPosX() + (max_text_width - text_width)) / 2);
            TextEx(line.c_str(), NULL, 0);
        }
    }
};