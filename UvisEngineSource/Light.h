#pragma once

class Component {
public:
    virtual void update() = 0;  // Abstract update function
    virtual ~Component() = default;
};

class LightComponent : public Component {
public:
    float color[3];
    float intensity;

    LightComponent() {
        color[0] = color[1] = color[2] = 1.0f;
        intensity = 1.0f;
    }

    void update() override {
        // Handle updates for light (optional for now)
    }
};