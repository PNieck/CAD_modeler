#version 410 core

layout (vertices=4) out;

uniform mat4 MVP;


void main()
{
    // Pass positions through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        const int MIN_TESS_LEVEL = 1;
        const int MAX_TESS_LEVEL = 16;

        // Step 1: Project all control points
        vec4 proj0 = MVP * gl_in[0].gl_Position;
        vec4 proj1 = MVP * gl_in[1].gl_Position;
        vec4 proj2 = MVP * gl_in[2].gl_Position;
        vec4 proj3 = MVP * gl_in[3].gl_Position;

        // Step 2: Get max value in all directions
        float maxX = max(max(proj0.x, proj1.x), max(proj2.x, proj3.x));
        float maxY = max(max(proj0.y, proj1.y), max(proj2.y, proj3.y));
        float maxZ = max(max(proj0.z, proj1.z), max(proj2.z, proj3.z));
        
        // Step 3: Get min value in all directions
        float minX = min(min(proj0.x, proj1.x), min(proj2.x, proj3.x));
        float minY = min(min(proj0.y, proj1.y), min(proj2.y, proj3.y));
        float minZ = min(min(proj0.z, proj1.z), min(proj2.z, proj3.z));

        // Step 4: Get distances along all directions clamped to clip space
        float xDist = clamp(maxX, -1.0f, 1.0f) - clamp(minX, -1.0f, 1.0f);
        float yDist = clamp(maxY, -1.0f, 1.0f) - clamp(minY, -1.0f, 1.0f);
        float zDist = clamp(maxZ, -1.0f, 1.0f) - clamp(minZ, -1.0f, 1.0f);

        // Step 5: Get max len
        float maxLen = max(xDist, max(yDist, zDist));

        // Step 6: Scale maxLen to 0.0, 1.0 range
        maxLen = maxLen / 2.0f + 0.5f;

        // Step 7: Determinate tesselation level
        gl_TessLevelOuter[0] = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, maxLen);
        gl_TessLevelOuter[1] = 16;
    }
}
