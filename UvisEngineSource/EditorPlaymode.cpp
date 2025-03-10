
#include "Application.h"
#include "Core.h"
#include "Editor.h"
#include "EditorPlaymode.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui.h"

    static bool isPlayModeActive = false;

    void StartPlayMode(bool* isOpen) {
     //   isPlayModeActive = true;
        if (!(*isOpen)) return; // Don't render if closed
        ImGui::Begin("Game");
        
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }



