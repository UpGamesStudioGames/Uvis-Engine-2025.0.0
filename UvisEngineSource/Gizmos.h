#pragma once
#include "imgui.h"

class Gizmo {
public:
    static void DrawGizmo(ImVec2& position, float& scale);
    static void HandleGizmoInput(ImVec2& position, float& scale);
};
