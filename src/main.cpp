#include <stdio.h>
#include <string>
#include <tuple>
#include <SDL2/SDL.h>
#include <getopt.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#define JSON_DIAGNOSTICS 1
#include <glad/glad.h>
#include <nlohmann/json.hpp>

#include "style.hpp"
#include "process.hpp"
#include "imGuiTextCentered.hpp"
#include "imGuiExtras.hpp"
#include "log.hpp"

const ImColor nav_start = ImColor(0.26f, 0.59f, 0.98f, 1.00f);
const ImColor nav_end = ImColor(0.35f, 0.40f, 0.61f, 0.62f);

void Breathing(float s)
{
    ImGuiStyle *style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    ImColor navhigh = ImLerp(nav_start.Value, nav_end.Value, s);
    colors[ImGuiCol_NavHighlight] = navhigh;
}

SDL_Window *sdl_win = NULL;
SDL_GLContext sdl_glctx = NULL;
const char *glsl_version = NULL;

const char* glsl_version_es_20 = "#version 100";
const char* glsl_version_core_32 = "#version 150";
static int create_gl_context(int width, int height, bool is_es2)
{
    if (is_es2) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        glsl_version = glsl_version_es_20;
    } else {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        glsl_version = glsl_version_core_32;
    }

    sdl_win = SDL_CreateWindow("Progress", 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!sdl_win) {
        fprintf(stderr, "Unable to create window: %s. Exiting\n", SDL_GetError());
        return 0;
    }

    sdl_glctx = SDL_GL_CreateContext(sdl_win);
    if (!sdl_glctx) {
        fprintf(stderr, "Warning: %s\n", SDL_GetError());
        return 0;
    }

    SDL_GL_MakeCurrent(sdl_win, sdl_glctx);
    return 1;
}

int main(int argc, char *argv[])
{
    int opt;
    const char *log_output = NULL;
    const char *font = NULL;
    const char *script = NULL;
    const char *titlearg = NULL;
    int script_flag = 0;

    static struct option long_options[] = {
        {"log", required_argument, 0, 'l'},
        {"font", required_argument, 0, 'f'},
        {"title", required_argument, 0, 't'},
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "f:s:t:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'l':
                log_output = optarg;
                break; 
            case 'f':
                font = optarg;
                break;
            case 't':
                titlearg = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [--font=font] [--title=string] [--log=file] command\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!titlearg) {
        titlearg = "Lorem Ipsum.";
    }

    if (optind < argc) {
        script = argv[optind];
    } else {
        fprintf(stderr, "Error: A command must be specified.\n");
        exit(EXIT_FAILURE);
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    int dw, dh;
    SDL_DisplayMode dp;
    if (SDL_GetDesktopDisplayMode(0, &dp) == 0) {
        dw = dp.w;
        dh = dp.h;
    }

    if (!create_gl_context(dw, dh, 0) && !create_gl_context(dw, dh, 1)) {
        fprintf(stderr, "No GL Context succeeded. Exiting\n");
        exit(-1);
    }

    if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
    {
        fprintf(stderr, "GLAD failed! Could not load OpenGL library. Exiting\n");
        exit(-1);
    }

    SDL_GL_SetSwapInterval(1);
    SDL_GetWindowSize(sdl_win, &dw, &dh);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    ImGuiStyle& style = ImGui::GetStyle();
    io.IniFilename = NULL;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    g.NavDisableMouseHover = true;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(sdl_win, sdl_glctx);
    ImGui_ImplOpenGL3_Init(glsl_version);
    std::string cmd = std::string(argv[2]);
    
    ProgressStyle(font, dw);
    ProcessManager manager(std::string(script), log_output, [](const std::string &var) -> std::string {
        return "";
    }, {
        std::make_tuple("SCREEN_WIDTH", std::to_string(dw)),
        std::make_tuple("SCREEN_HEIGHT", std::to_string(dh))
    });

    std::string title = titlearg;

    int result = 0;
    int runs = 1;
    while (runs) {
        // Force Nav on every frame
        g.NavDisableHighlight = false;
        g.NavInitRequestFromMove = true;
        g.IO.NavVisible = true;
        g.IO.NavActive = true;

        Breathing(0.5f + sinf(SDL_GetTicks() / 250.f) / 2);

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            ImGui_ImplSDL2_ProcessEvent(&ev);

            switch (ev.type) {
            case SDL_QUIT:
                result = -1;
                runs = 0;
                break;
            }
        }

        manager.DrainPipe();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(
                nav_end.Value.x * nav_end.Value.w,
                nav_end.Value.y * nav_end.Value.w,
                nav_end.Value.z * nav_end.Value.w, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        const uint32_t win_flags =
              ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoCollapse;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(sdl_win);
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(16, 16));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 32, io.DisplaySize.y - 40));
        ImGui::Begin(title.c_str(), NULL, win_flags);

        bool spins = manager.tags.empty();
        RenderMessages(manager.GetLines(), spins);

        ImGui::End();

        // We're waiting for user input on something...
        if (!manager.tags.empty()) {
            nlohmann::json &tag = *manager.tags.begin();
            if (tag["type"] == "settitle") {
                title = tag["value"];
                manager.tags.erase(manager.tags.begin());
            } else {
                std::string win_title = tag["title"].get<std::string>() + "##sub";
                ImGui::SetNextWindowPos(g.IO.DisplaySize * 0.5f, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::Begin(win_title.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
                ImGuiID window = ImGui::GetCurrentWindow()->ChildId;

                if (ImGui::IsWindowAppearing() || !ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) || g.NavId == NULL) {
                    ImGui::SetWindowFocus();
                    ImGui::SetKeyboardFocusHere();
                }

                if (tag["type"] == "confirm" || tag["type"] == "msgbox") {
                    if (ImGui::IsWindowAppearing())
                        ImGui::SetHoveredID(ImGui::GetCurrentWindow()->GetID("Yes"));

                    std::string confirm = tag["message"];
                    ImGui::TextCentered(confirm);

                    bool y, n = false;
                    if (tag["type"] == "confirm") {
                        ImGui::CenterButtons("Yes", "No");
                        y = ImGui::Button("Yes");
                        ImGui::SameLine();
                        n = ImGui::Button("No");
                    } else {
                        ImGui::CenterButtons("Yes");
                        y = ImGui::Button("Yes");
                    }

                    if (!ImGui::IsWindowAppearing() && (y || n)) {
                        manager.Reply("%d\n", y == true);
                        manager.tags.erase(manager.tags.begin());
                    }
                }

                ImGui::End();
            }
        }

        if (!manager.IsRunning(result))
            runs = 0;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(sdl_win);
    }

    if (result != 0) {
        printf("## ERRORLOG:\n%s\n", manager.GetLogFinal().c_str());
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    SDL_Quit();
    return result;
}