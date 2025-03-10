#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include <chrono>
#include <fstream>
#include "EditorPlaymode.h"                  
#include "imnodes/imnodes.h"
#include "imnodes/imnodes_internal.h"
#include "Editor.h"
#include "MainComponent.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ScriptEngine.h"
#include "Objects.h"

#include "Objects-sphere.h"
std::string scriptContent = "// Write your script here...\nvoid Update() {\n    // Your code\n}";


void renderSkybox();
void loadSkyboxTexture();


class Script : public Component {
public:
    std::string scriptCode;  
    
    Script() : scriptCode("// Enter your script here...\n") {}

    void update() override {
    

    }
};






class CubeRender : public Component {
public:
    float position[3];
    float rotation[3];
    
    CubeRender() {
        position[0] = position[1] = position[2] = 0.0f;
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
    }

    void update() override {
     
    }
};
class MeshComponent : public Component {
public:

  

    void update() override {
     
    }
};
class PointLight : public Component {
public:
    float color[3];
    float intensity;
    float rotation[3];

    PointLight() {
        color[0] = color[1] = color[2] = 1.0f;
        intensity = 1.0f;
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
    }

    void update() override {
 
    }
};

class DirectionalLight : public Component {
public:
    float color[3];
    float intensity;

    DirectionalLight() {
        color[0] = color[1] = color[2] = 1.0f;
        intensity = 1.0f;
    }

    void update() override {

    }
};

struct Light {
    float position[3];
    float color[3];
    float intensity;
};

Light light = { { 0.0f, 5.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, 1.0f };

Light LightExit = { { 0.0f, 5.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, 1.0f };

Light* Baked;
Light* Realtime;

std::vector<std::string> logMessages;
int logIndex = 1;

class GameObject {
public:
    std::string name;
    bool isPrefab;
    bool isLight;
    std::string status;  
    std::vector<Component*> components;

    GameObject(const std::string& name, bool isPrefab = false, bool isLight = false)
        : name(name), isPrefab(isPrefab), isLight(isLight), status("Active") {} 

    template <typename T>
    void add_component() {
        components.push_back(new T());
    }

    template <typename T>
    T* get_component() {
        for (auto& component : components) {
            T* result = dynamic_cast<T*>(component);
            if (result) return result;
        }
        return nullptr;
    }
};



class Camera {
public:
    float position[3];
    float rotation[3];  

    Camera() {
        position[0] = position[1] = position[2] = 0.0f;
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
    }

    void apply() {
      
        glRotatef(rotation[0], 1.0f, 0.0f, 0.0f); 
        glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);  
        glRotatef(rotation[2], 0.0f, 0.0f, 1.0f);  
        glTranslatef(position[0], position[1], position[2]); 

    }
};

Camera camera;



std::vector<GameObject*> sceneObjects;  
std::unordered_map<std::string, GameObject*> prefabs; 
GameObject* selectedObject = nullptr;


bool object_exists(const std::string& name) {
    for (auto& obj : sceneObjects) {
        if (obj->name == name) {
            return true;
        }
    }
    return false;
}


std::string generate_unique_name(const std::string& baseName) {
    std::string newName = baseName;
    int counter = 1;
    while (object_exists(newName)) {
        newName = baseName + std::to_string(counter);
        counter++;
    }
    return newName;
}


void create_sample_objects() {
   
 
    GameObject* cube = new GameObject("Cube1");
    cube->add_component<CubeRender>();
    cube->add_component<MeshComponent>();
 //   cube->add_component<Script>();
    sceneObjects.push_back(cube);



    GameObject* lightObj = new GameObject("Directional Light", false, true);
    lightObj->add_component<DirectionalLight>(); 
    sceneObjects.push_back(lightObj);

    GameObject* pointLight = new GameObject("Point Light", false, true);
    pointLight->add_component<PointLight>();  
    sceneObjects.push_back(pointLight);

   
    prefabs["CubePrefab"] = new GameObject("CubePrefab", true);
    prefabs["SpherePrefab"] = new GameObject("SpherePrefab", true);
}

std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm buf;
    localtime_s(&buf, &time);

    char time_str[9];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", &buf);
    return std::string(time_str);
}

void log_message(const std::string& category, const std::string& message) {
    std::string timestamp = get_current_time();
    logMessages.push_back(timestamp + "::" + std::to_string(logIndex++) + "::" + category + " - " + message);
}

void clear_console() {
    logMessages.clear();
    logIndex = 0;
}

GLFWwindow* init_window(int windowWidth, int windowHeight, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    int iconWidth, iconHeight, channels;
    unsigned char* pixels = stbi_load("assets/WindowsX64Icon/OpenGLWindow/UvisIcon.png", &iconWidth, &iconHeight, &channels, 0);
    if (pixels) {
        GLFWimage icon = { iconWidth, iconHeight, pixels };
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(pixels);
    }
    else {
        std::cerr << "Failed to load icon" << std::endl;
    }
    return window;
}

bool meshRenderMode = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        meshRenderMode = !meshRenderMode; 
    }
}




void render_scene()
{

    glViewport(0, 0, 1280, 720);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);



    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLfloat light_position[] = { light.position[0], light.position[1], light.position[2], 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    for (auto& obj : sceneObjects) {
        if (obj->isLight) {
            DirectionalLight* lightComp = obj->get_component<DirectionalLight>();
            if (lightComp) {
                GLfloat light_color[] = {
                    lightComp->color[0] * lightComp->intensity,
                    lightComp->color[1] * lightComp->intensity,
                    lightComp->color[2] * lightComp->intensity,
                    1.0f
                };
                glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
                glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
                glEnable(GL_LIGHT0);
            }
        }
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = 1280.0f / 720.0f;
    float fov = 45.0f;
    float nearPlane = 0.1f, farPlane = 100.0f;
    float top = nearPlane * tanf(fov * 0.5f * (3.14159265f / 180.0f));
    float right = top * aspect;
    glFrustum(-right, right, -top, top, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

    loadSkyboxTexture();
    renderSkybox();

    glPushMatrix();
    for (auto& obj : sceneObjects) {
        CubeRender* transform = obj->get_component<CubeRender>();
        if (transform) {
            glTranslatef(transform->position[0], transform->position[1], transform->position[2]);
            glRotatef(transform->rotation[0], 1.0f, 0.0f, 0.0f);
            glRotatef(transform->rotation[1], 0.0f, 1.0f, 0.0f);
            glRotatef(transform->rotation[2], 0.0f, 0.0f, 1.0f);
            ObjectRender::renderCCPCube();
        }

        if (meshRenderMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    glPopMatrix();

    glDisable(GL_LIGHTING);
}








void DrawGizmo(ImVec2 position, ImVec4 color);
void HandleGizmoInput(ImVec2& position);
void package_manager(GLFWwindow* window);
GLuint skyboxTexture;
void loadSkyboxTexture() {
    int width, height, channels;


    unsigned char* data = stbi_load("skybox.png", &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Failed to load texture" << std::endl;
        return;
    }

   // std::cout << "Texture loaded! Dimensions: " << width << "x" << height << ", Channels: " << channels << std::endl;

  
    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

  
    if (channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        std::cerr << "Unknown texture format" << std::endl;
        stbi_image_free(data);
        return;
    }


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    stbi_image_free(data);
}


void renderSkybox() {
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

    glBegin(GL_QUADS);


    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -10.0f, -10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, -10.0f, -10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, 10.0f, -10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 10.0f, -10.0f);


    glTexCoord2f(0.0f, 0.0f); glVertex3f(10.0f, -10.0f, 10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, -10.0f, 10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 10.0f, 10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(10.0f, 10.0f, 10.0f);

 
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -10.0f, 10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, -10.0f, -10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 10.0f, -10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 10.0f, 10.0f);


    glTexCoord2f(0.0f, 0.0f); glVertex3f(10.0f, -10.0f, -10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, -10.0f, 10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, 10.0f, 10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(10.0f, 10.0f, -10.0f);


    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 10.0f, -10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, 10.0f, -10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, 10.0f, 10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 10.0f, 10.0f);


    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -10.0f, -10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, -10.0f, -10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, -10.0f, 10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, -10.0f, 10.0f);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);
}


bool showPackageManager = false;


char searchBuffer[64] = "";


struct Package {
    std::string name;
    bool installed;
};

std::vector<Package> packages = {
      
    {"Baked Module", false},
    {"Realtime Module", true},

};

void ShowPackageManager() {
    if (!showPackageManager) return;

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Light Manager", &showPackageManager, ImGuiWindowFlags_NoCollapse);

   
 

    ImGui::End();
}

bool showLightingManager = true;

void renderLightingManager(int) {
    if (showLightingManager) {
        if (!showPackageManager) return;

  
        ImGui::Begin("Light Manager", &showPackageManager, ImGuiWindowFlags_NoCollapse);


       


     

        ImGui::End();
    }
}


void render_ui(GLFWwindow* window) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.WindowBorderSize = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

    render_scene();

    ImGui::Begin("Performance");
    static double lastTime = glfwGetTime();
    static int frameCount = 0;
    static float fps = 0.0f;

    double currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - lastTime >= 1.0) {
        fps = frameCount / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "FPS: %.0f", fps);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "GPU: Unknown");
    ImGui::End();

    ImGui::Begin("Project");
    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Console")) {
            if (ImGui::Button("Clear Console")) {
                clear_console();
            }
            for (const auto& msg : logMessages) {
                ImGui::TextUnformatted(msg.c_str());
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Content Explorer")) {
            ImGui::Text("Content Explorer");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::Begin("Hierarchy");
    for (auto& obj : sceneObjects) {
        if (ImGui::TreeNode(obj->name.c_str())) {
            if (ImGui::IsItemClicked()) {
                selectedObject = obj;
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save")) {
                log_message("Saved Project Files", std::to_string(203));
            }
            if (ImGui::MenuItem("Exit")) {
                glfwTerminate();
                return;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Light Manager")) {
                showPackageManager = !showPackageManager;
            }
         
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Inspector");
    if (selectedObject != nullptr) {
        CubeRender* transform = selectedObject->get_component<CubeRender>();
        if (transform) {
            ImGui::Text("Cube Render Settings");
            ImGui::SliderFloat3("Position", transform->position, -10.0f, 10.0f);
            ImGui::SliderFloat3("Rotation", transform->rotation, 0.0f, 360.0f);
        }

        ImGui::Text("Status: %s", selectedObject->status.c_str());

        Script* ScriptedObject = selectedObject->get_component<Script>();
        if (ScriptedObject) {
            ImGui::Text("Scripted Game Object");
        }

        DirectionalLight* lightComp = selectedObject->get_component<DirectionalLight>();
        if (lightComp) {
            ImGui::Text("Light Settings");
            ImGui::ColorEdit3("Color", lightComp->color);
            ImGui::SliderFloat("Intensity", &lightComp->intensity, 0.0f, 10.0f);
        }
        PointLight* pointLight = selectedObject->get_component<PointLight>();
        if (pointLight) {
            ImGui::Text("Point Light Settings");
            ImGui::ColorEdit3("Point Color", pointLight->color);
            ImGui::SliderFloat("Point Intensity", &pointLight->intensity, 0.0f, 10.0f);
        }

        if (ImGui::IsMouseClicked(1)) {
            ImGui::OpenPopup("ComponentPopup");
        }

        if (ImGui::BeginPopupContextWindow("ComponentPopup")) {
            if (selectedObject) {
                ImGui::SetWindowFontScale(1.5f);

                ImGui::Text("Componenets");


                ImGui::SetWindowFontScale(1.0f);

                if (ImGui::MenuItem("Cube Render")) {
                    selectedObject->add_component<CubeRender>();
                }
                if (ImGui::MenuItem("Directional Light")) {
                    selectedObject->add_component<DirectionalLight>();
                }
                if (ImGui::MenuItem("Point Light")) {
                    selectedObject->add_component<PointLight>();
                }
                if (ImGui::MenuItem("Mesh Render")) {
                    selectedObject->add_component<MeshComponent>();
                }
                if (ImGui::MenuItem("Script")) {
                    selectedObject->add_component<Script>();
                }
                if (ImGui::MenuItem("Debug Test")) {
                    log_message("Debugger:::::: SUS", std::to_string(1));

                }
            }
            
            ImGui::EndPopup();
        }
    }
    ImGui::End();

    ShowPackageManager(); 

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void PlayModeRender() {
  
}
void package_manager(GLFWwindow* window) {




    ImGui::Begin("Package Manager", &showPackageManager);
    ImGui::End();


}

void scripting();

void scripting() {
    ImGui::Begin("Script Editor");

    static char buffer[4096];
    strncpy_s(buffer, scriptContent.c_str(), sizeof(buffer));

    ImGui::InputTextMultiline("##ScriptEditor", buffer, sizeof(buffer), ImVec2(500, 400));


    if (ImGui::Button("Save")) {
        scriptContent = buffer; 
        
    }

    ImGui::End();
}

bool isDragging = false;     
ImVec2 dragOffset(0, 0);       




int selectedAxis = -1; 


int main() {
    GLFWwindow* window = init_window(1280, 720, "Uvis Engine - < 2025.1.0 > - Main Project");
    
    create_sample_objects();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        render_scene();
        render_ui(window);


        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}