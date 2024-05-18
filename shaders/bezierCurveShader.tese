#version 410 core

layout (isolines, equal_spacing) in;

uniform mat4 MVP;


void main()
{
    // float t = gl_TessCoord.x;
    float t = gl_TessCoord.x / gl_TessLevelOuter[0] + gl_TessCoord.y;
    float oneMinusT = (1.0f - t);

    // Control points
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    // Bernstein polynomials
    float b0 = oneMinusT * oneMinusT * oneMinusT;
    float b1 = 3 * oneMinusT * oneMinusT * t;
    float b2 = 3 * oneMinusT * t * t;
    float b3 = t * t * t;

    vec3 pos = p0*b0 + p1*b1 + p2*b2 + p3*b3;

    gl_Position = MVP * vec4(pos, 1.0f);
}