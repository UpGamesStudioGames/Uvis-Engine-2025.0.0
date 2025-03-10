#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

void PlayModeRender(); // Declare the function

#endif

class Component {
public:
    virtual void update() = 0;  // Abstract update function
    virtual ~Component() = default;
};

// TransformComponent class (handles position and rotation)
class TransformComponent : public Component {
public:
    float position[3];
    float rotation[3];

    TransformComponent() {
        position[0] = position[1] = position[2] = 0.0f;
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
    }

    void update() override {
        // Handle updates for transform (optional for now)
    }
};
class MeshComponent : public Component {
public:



    void update() override {
        // Handle updates for transform (optional for now)
    }
};

// LightComponent class (handles light settings)
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

class SpriteRender : public Component {
public:
    float SpriteTexture;


    SpriteRender() {
      
    }

    void update() override {
        // Handle updates for light (optional for now)
    }
};
