#pragma once

class MeshData {

};

class Component {
public:
    virtual void update() = 0;  // Abstract update function
    virtual ~Component() = default;
};
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