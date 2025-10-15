#version 410 core

layout (location = 0) in vec3 aPos;


// Model View Perspective matrix
uniform mat4 MVP;

uniform sampler2D heightMap;

uniform float heightMapXLen;
uniform float heightMapZLen;

uniform vec3 mainHeightMapCorner;

out vec3 worldPos;
out vec3 normal;


vec3 PointPosition(vec3 pos)
{
    vec2 texCoord;
    texCoord.x = (pos.x - mainHeightMapCorner.x) / heightMapXLen;
    texCoord.y = (pos.z - mainHeightMapCorner.z) / heightMapZLen;

    pos.y += texture(heightMap, texCoord).r;

    return pos;
}


void main()
{
    ivec2 texSize = textureSize(heightMap, 0);
    float pixelXLen = heightMapXLen / float(texSize.x);
    float pixelZLen = heightMapZLen / float(texSize.y);

    worldPos = PointPosition(aPos);

    vec3 xShifted = aPos;
    xShifted.x += pixelXLen;
    xShifted = PointPosition(xShifted);

    vec3 zShifted = aPos;
    zShifted.z += pixelZLen;
    zShifted = PointPosition(zShifted);

    vec3 tangent = normalize(xShifted - worldPos);
    vec3 bitangent = normalize(zShifted - worldPos);

    normal = cross(bitangent, tangent);

    gl_Position = MVP * vec4(worldPos, 1.0);
}
