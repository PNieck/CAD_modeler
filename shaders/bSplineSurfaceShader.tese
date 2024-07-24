#version 410 core

layout (isolines, equal_spacing) in;

uniform mat4 MVP;


float a(int n, int j, float t) {
    return (float(j+n-1) - t)/float(n);
}


float b(int n, int j, float t) {
    return (t - float(j - 1))/float(n);
}


vec4 CubicBSplinesBaseFunctions(float t) {
    float N[4];
    N[0] = 1;

    N[1] = b(1, 1, t) * N[0];
    N[0] = a(1, 1, t) * N[0];

    N[2] = b(2, 1, t) * N[1];
    N[1] = b(2, 0, t) * N[0] + a(2, 1, t) * N[1];
    N[0] = a(2, 0, t) * N[0];

    N[3] = b(3, 1, t) * N[2];
    N[2] = b(3, 0, t) * N[1] + a(3, 1, t) * N[2];
    N[1] = b(3, -1, t) * N[0] + a(3, 0, t) * N[1];
    N[0] = a(3, -1, t) * N[0];

    return vec4 (
        N[0],
        N[1],
        N[2],
        N[3]
    );
}


void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    v *= float(gl_TessLevelOuter[0]) / float(gl_TessLevelOuter[0] - 1);

    // Bernstein polynomials
    vec4 Nu = CubicBSplinesBaseFunctions(u);
    vec4 Nv = CubicBSplinesBaseFunctions(v);

    // Control points
    vec3 p00 = gl_in[0].gl_Position.xyz;
    vec3 p01 = gl_in[1].gl_Position.xyz;
    vec3 p02 = gl_in[2].gl_Position.xyz;
    vec3 p03 = gl_in[3].gl_Position.xyz;

    vec3 p10 = gl_in[4].gl_Position.xyz;
    vec3 p11 = gl_in[5].gl_Position.xyz;
    vec3 p12 = gl_in[6].gl_Position.xyz;
    vec3 p13 = gl_in[7].gl_Position.xyz;

    vec3 p20 = gl_in[8].gl_Position.xyz;
    vec3 p21 = gl_in[9].gl_Position.xyz;
    vec3 p22 = gl_in[10].gl_Position.xyz;
    vec3 p23 = gl_in[11].gl_Position.xyz;

    vec3 p30 = gl_in[12].gl_Position.xyz;
    vec3 p31 = gl_in[13].gl_Position.xyz;
    vec3 p32 = gl_in[14].gl_Position.xyz;
    vec3 p33 = gl_in[15].gl_Position.xyz;


    vec3 pos = p00 * Nu[0] * Nv[0];
    pos += p01 * Nu[0] * Nv[1];
    pos += p02 * Nu[0] * Nv[2];
    pos += p03 * Nu[0] * Nv[3];

    pos += p10 * Nu[1] * Nv[0];
    pos += p11 * Nu[1] * Nv[1];
    pos += p12 * Nu[1] * Nv[2];
    pos += p13 * Nu[1] * Nv[3];

    pos += p20 * Nu[2] * Nv[0];
    pos += p21 * Nu[2] * Nv[1];
    pos += p22 * Nu[2] * Nv[2];
    pos += p23 * Nu[2] * Nv[3];

    pos += p30 * Nu[3] * Nv[0];
    pos += p31 * Nu[3] * Nv[1];
    pos += p32 * Nu[3] * Nv[2];
    pos += p33 * Nu[3] * Nv[3];

    gl_Position = MVP * vec4(pos, 1.0f);
}
