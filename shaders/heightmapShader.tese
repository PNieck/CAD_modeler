#version 410 core

layout (quads, equal_spacing, ccw) in;

uniform mat4 MVP;
uniform sampler2D heightMap;

uniform vec3 mainHeightMapCorner;
uniform float heightMapXLen;
uniform float heightMapZLen;


void main()
{
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // ----------------------------------------------------------------------
    // retrieve input quad coordinates
    vec3 q00 = gl_in[0].gl_Position.xyz;
    vec3 q01 = gl_in[1].gl_Position.xyz;
    vec3 q10 = gl_in[2].gl_Position.xyz;
    vec3 q11 = gl_in[3].gl_Position.xyz;

    // bilinearly interpolate texture coordinate across patch
    vec3 q0 = (q01 - q00) * u + q00;
    vec3 q1 = (q11 - q10) * u + q10;
    vec3 pointCoord = (q1 - q0) * v + q0;

    float pointX = abs(pointCoord.x - mainHeightMapCorner.x);
    float pointZ = abs(pointCoord.z - mainHeightMapCorner.z);

    vec2 texCoord;
    texCoord.x = pointX / heightMapXLen;
    texCoord.y = pointZ / heightMapZLen;

    // lookup texel at patch coordinate for height and scale + shift as desired
    pointCoord.y += texture(heightMap, texCoord).r;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = MVP * vec4(pointCoord, 1.f);
}