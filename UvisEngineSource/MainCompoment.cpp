#include "MainComponent.h"
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "ScriptEngine.h"
#include "Editor.h"
void ScriptEngine::CreateScriptingCompoment() {
    std::cout << "CreateScriptingCompoment" << std::endl;
    if (ScriptEngine::GetSystemContex) {
        GLFWwindow* window;
       // glfwDestroyWindow(window);
        glfwTerminate();

    }
   
  //  ImGui::Render();


  //  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void ScriptEngine::GetSystemContex() {
    std::cout << "Contex Recieved" << std::endl;

    if (ScriptEngine::CreateScriptingCompoment) {
        
    }
}
void ScriptEngine::ReloadAssembly() {
    if (ScriptEngine::GetSystemContex) {
        std::cout << "Got System Contex" << std::endl;
    }
}