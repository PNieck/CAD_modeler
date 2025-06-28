#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uvIn;

// Model View Perspective matrix
uniform mat4 MVP;

out vec2 uvCoord;

void main()
{
    uvCoord = uvIn;
    gl_Position = MVP * vec4(aPos, 1.0);
}
