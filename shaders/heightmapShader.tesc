#version 410 core


layout (vertices=4) out;

uniform mat4 MVP;
uniform float projection00;


void main()
{
    // Pass positions through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        // Source: https://developer.nvidia.com/content/dynamic-hardware-tessellation-basics

        const int MIN_TESS_LEVEL = 1;
        const int MAX_TESS_LEVEL = 64;

        float d0 = length(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz);
        float d1 = length(gl_in[1].gl_Position.xyz - gl_in[2].gl_Position.xyz);
        float d2 = length(gl_in[2].gl_Position.xyz - gl_in[3].gl_Position.xyz);
        float d3 = length(gl_in[3].gl_Position.xyz - gl_in[0].gl_Position.xyz);

        vec4 midP0 = (gl_in[0].gl_Position + gl_in[1].gl_Position) / 2.f;
        vec4 midP1 = (gl_in[1].gl_Position + gl_in[2].gl_Position) / 2.f;
        vec4 midP2 = (gl_in[2].gl_Position + gl_in[3].gl_Position) / 2.f;
        vec4 midP3 = (gl_in[3].gl_Position + gl_in[0].gl_Position) / 2.f;

        vec4 clipMidP0 = MVP * midP0;
        vec4 clipMidP1 = MVP * midP1;
        vec4 clipMidP2 = MVP * midP2;
        vec4 clipMidP3 = MVP * midP3;

        float D0 = abs((d0 * projection00) / clipMidP0.w);
        float D1 = abs((d1 * projection00) / clipMidP1.w);
        float D2 = abs((d2 * projection00) / clipMidP2.w);
        float D3 = abs((d3 * projection00) / clipMidP3.w);

        float tessLevel0 = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, D0);
        float tessLevel1 = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, D1);
        float tessLevel2 = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, D2);
        float tessLevel3 = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, D3);

        // Step 5: set the corresponding outer edge tessellation levels
        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        // Step 6: set the inner tessellation levels to the max of the two parallel edges
        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}
