#version 410 core

layout (isolines, equal_spacing) in;

uniform mat4 MVP;


vec4 CubicBernsteinPolynomials(float t) {
    float oneMinusT = 1.0 - t;

    return vec4 (
        oneMinusT * oneMinusT * oneMinusT,
        3.f * oneMinusT * oneMinusT * t,
        3.f * oneMinusT * t * t,
        t * t * t
    );
}


void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // Bernstein polynomials
    vec4 bu = CubicBernsteinPolynomials(u);
    vec4 bv = CubicBernsteinPolynomials(v);

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

    // Result of bu*p
    vec3 a0 = bu.x*p00 + bu.y*p10 + bu.z*p20 + bu.w*p30;
    vec3 a1 = bu.x*p01 + bu.y*p11 + bu.z*p21 + bu.w*p31;
    vec3 a2 = bu.x*p02 + bu.y*p12 + bu.z*p22 + bu.w*p32;
    vec3 a3 = bu.x*p03 + bu.y*p13 + bu.z*p23 + bu.w*p33;

    // pos = bu * p * bv^T
    vec3 pos = a0*bv.x + a1*bv.y + a2*bv.z + a3*bv.w;

    gl_Position = MVP * vec4(pos, 1.0f);
}
