class Component {
public:
    virtual void update() = 0;  // Abstract update function
    virtual ~Component() = default;
};