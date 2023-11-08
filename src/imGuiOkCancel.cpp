#include <functional>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include <nlohmann/json.hpp>
#include "style.h"


void OkCancel(const nlohmann::json &tag)
{
    ImGui::SetNextWindowPos(g.IO.DisplaySize * 0.5f, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Confirm");

    std::string confirm = tag["message"];

    ImGui::TextUnformatted(confirm.c_str());

    float btn_y = ImGui::GetCursorPosY() + pad_y + 8.f;
    ImVec2 btn1 = ImVec2(btn_ok_pos_x, btn_y);
    ImVec2 btn2 = ImVec2(btn_cancel_pos_x, btn_y);
    
    if (ImGui::IsWindowAppearing()) {
        ImGui::SetWindowFocus();
        ImGui::SetKeyboardFocusHere();
    }

    bool ok = false, cancel = false;
    ImGui::SetCursorPos(btn1);
    ok = ImGui::Button("OK");
    if (has_cancel) {
        ImGui::SetCursorPos(btn2);
        cancel = ImGui::Button("Cancel");
    }

    ImGui::End();
}