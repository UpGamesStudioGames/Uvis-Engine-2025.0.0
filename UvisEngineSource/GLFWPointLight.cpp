#include <iostream>
#include "GLFW/glfw3.h"

// << point light >> \\

struct PointLight {
    float position[3];
    float color[3];
    float intensity;
};




void RenderPointLight() {

   
PointLight pointlight = { { 0.0f, 5.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, 1.0f };

}