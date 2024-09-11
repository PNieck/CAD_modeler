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

    v *= float(gl_TessLevelOuter[0]) / float(gl_TessLevelOuter[0] - 1);

    // Bernstein polynomials
    vec4 bu = CubicBernsteinPolynomials(u);
    vec4 bv = CubicBernsteinPolynomials(v);

    // Control points
    vec3 _00 = gl_in[0].gl_Position.xyz;
    vec3 _01 = gl_in[1].gl_Position.xyz;
    vec3 _02 = gl_in[2].gl_Position.xyz;
    vec3 _03 = gl_in[3].gl_Position.xyz;

    vec3 _13 = gl_in[4].gl_Position.xyz;
    vec3 _23 = gl_in[5].gl_Position.xyz;

    vec3 _33 = gl_in[6].gl_Position.xyz;
    vec3 _32 = gl_in[7].gl_Position.xyz;
    vec3 _31 = gl_in[8].gl_Position.xyz;
    vec3 _30 = gl_in[9].gl_Position.xyz;

    vec3 _20 = gl_in[10].gl_Position.xyz;
    vec3 _10 = gl_in[11].gl_Position.xyz;

    vec3 n01 = gl_in[12].gl_Position.xyz;
    vec3 n02 = gl_in[13].gl_Position.xyz;

    vec3 n13 = gl_in[14].gl_Position.xyz;
    vec3 n23 = gl_in[15].gl_Position.xyz;

    vec3 n32 = gl_in[16].gl_Position.xyz;
    vec3 n31 = gl_in[17].gl_Position.xyz;

    vec3 n20 = gl_in[18].gl_Position.xyz;
    vec3 n10 = gl_in[19].gl_Position.xyz;

    vec3 f0 = (u * n31 + v*n20);
    float denominator = u + v;
    if (denominator != 0.f)
        f0 /= denominator;

    vec3 f1 = ((1-u)*n32 + v*n23);
    denominator = 1 - u + v;
    if (denominator != 0.f)
        f1 /= denominator;

    vec3 f2 = ((1-u)*n02 + (1-v)*n13);
    denominator = 2 - u - v;
    if (denominator != 0.f)
        f2 /= denominator;

    vec3 f3 = (u*n01 + (1-v)*n10);
    denominator = 1 + u - v;
    if (denominator != 0.f)
        f3 /= denominator;

    // Result of bu*p
    vec3 a0 = bu.x*_30 + bu.y*_31 + bu.z*_32 + bu.w*_33;
    vec3 a1 = bu.x*_20 + bu.y*f0  + bu.z*f1  + bu.w*_23;
    vec3 a2 = bu.x*_10 + bu.y*f3  + bu.z*f2  + bu.w*_13;
    vec3 a3 = bu.x*_00 + bu.y*_01 + bu.z*_02 + bu.w*_03;

    // pos = bu * p * bv^T
    vec3 pos = a0*bv.x + a1*bv.y + a2*bv.z + a3*bv.w;

    gl_Position = MVP * vec4(pos, 1.0f);
}
