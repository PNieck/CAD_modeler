#version 410 core

// vertex position
layout (location = 0) in vec3 aPos;


void main()
{
    // convert XYZ vertex to XYZW homogeneous coordinate
    gl_Position = vec4(aPos, 1.0);
}
