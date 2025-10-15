#version 410 core

// Vertex in world position
layout (location = 0) in vec3 aPos;

// View Perspective matrix
uniform mat4 VP;
uniform vec3 globNormal;

out vec3 worldPos;
out vec3 normal;

void main()
{
    worldPos = aPos;
    normal = globNormal;

    gl_Position = VP * vec4(aPos, 1.0);
}
