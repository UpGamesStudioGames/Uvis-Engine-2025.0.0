#include "GLFW/glfw3.h"
#include <iostream>
#include <cmath>
#include "Objects-sphere.h"
#define M_PI 3.14159265358979323846

void Sphere::renderCPPSphere(float radius, unsigned int slices, unsigned int stacks) {
    for (unsigned int i = 0; i < stacks; ++i) {
        float stackAngle1 = M_PI / 2 - i * M_PI / stacks;    // Latitude angle for the first stack
        float stackAngle2 = M_PI / 2 - (i + 1) * M_PI / stacks;  // Latitude angle for the next stack

        float xy1 = radius * cosf(stackAngle1);  // Radius at this latitude
        float z1 = radius * sinf(stackAngle1);   // Z position for first stack
        float xy2 = radius * cosf(stackAngle2);  // Radius at next latitude
        float z2 = radius * sinf(stackAngle2);   // Z position for next stack

        // Loop through slices (longitude)
        for (unsigned int j = 0; j < slices; ++j) {
            float sliceAngle1 = j * 2 * M_PI / slices;   // Longitude angle for the first slice
            float sliceAngle2 = (j + 1) * 2 * M_PI / slices; // Longitude angle for the next slice

            // Vertices for the first stack
            float x1 = xy1 * cosf(sliceAngle1);
            float y1 = xy1 * sinf(sliceAngle1);
            float x2 = xy1 * cosf(sliceAngle2);
            float y2 = xy1 * sinf(sliceAngle2);

            // Vertices for the second stack
            float x3 = xy2 * cosf(sliceAngle1);
            float y3 = xy2 * sinf(sliceAngle1);
            float x4 = xy2 * cosf(sliceAngle2);
            float y4 = xy2 * sinf(sliceAngle2);

            // Draw the quads (two triangles per quad)
            glBegin(GL_QUADS);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z1);
            glVertex3f(x4, y4, z2);
            glVertex3f(x3, y3, z2);
            glEnd();
        }
    }
}