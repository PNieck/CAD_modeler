#version 410 core

uniform vec4 color;
uniform sampler2D parameterSpace;

in vec2 uvCoord;

out vec4 fragColor;


void main() {
    if (texture(parameterSpace, uvCoord).r == 0.f) {
        fragColor = vec4(0.f);
        return;
    }

    fragColor = color;
}
