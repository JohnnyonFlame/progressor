#include <math.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include "style.hpp"

ImVector<ImWchar> ranges_items;

float getRoundedSizes(float size)
{
    if (size >= 60.f)
        return 60.f;
    if (size >= 48.f)
        return 48.f;
    if (size >= 36.f)
        return 36.f;
    if (size >= 24.f)
        return 24.f;
    return 18.f;
}

static float ui_scale = 1.0f;

float GetUIScale()
{
    return ui_scale;
}

void ProgressStyle(const char *font, int width)
{
    ImGui::StyleColorsDark();
    ImGuiIO& io = GImGui->IO;
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
    float font_size = getRoundedSizes(18.f * ((float)width / 480.f));
    ui_scale = font_size / 18.f;

    printf("%f\n", font_size);

    if (font) {
        ImFontGlyphRangesBuilder builder_items;
        builder_items.AddRanges(io.Fonts->GetGlyphRangesDefault());
        builder_items.BuildRanges(&ranges_items);

        io.Fonts->AddFontFromFileTTF(font, font_size, NULL, ranges_items.Data);
        printf("Using font %s\n", font);
    } else {
        ImFontConfig font_cfg;
        font_cfg.SizePixels = font_size;
        io.Fonts->AddFontDefault(&font_cfg);
    }
}