#version 410 core

// vertex position
layout (location = 0) in vec3 aPos;

uniform sampler2D heightMap;

// View Perspective matrix
uniform mat4 VP;

uniform float heightMapXLen;
uniform float heightMapZLen;

uniform vec3 mainHeightMapCorner;

uniform vec3 inNormal;

uniform int sideType;

out vec3 worldPos;
out vec3 normal;

// Side types
const int positiveX = 0;
const int negativeX = 1;
const int positiveZ = 2;
const int negativeZ = 3;


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

    int xResolution = texSize.x;
    int zResolution = texSize.y;

    float pixelXLen = heightMapXLen / float(xResolution);
    float pixelZLen = heightMapZLen / float(zResolution);

    worldPos = aPos;
    if (sideType == positiveZ)
        worldPos.z = mainHeightMapCorner.z + zResolution*pixelZLen - pixelZLen/2.f;
    else if (sideType == negativeZ)
        worldPos.z = mainHeightMapCorner.z + pixelZLen/2.f;
    else if (sideType == positiveX)
        worldPos.x = mainHeightMapCorner.x + xResolution*pixelXLen - pixelXLen/2.f;
    else // if (sideType == negativeX)
        worldPos.x = mainHeightMapCorner.x + pixelXLen/2.f;

    if (gl_VertexID % 2 == 0)
        worldPos = PointPosition(worldPos);

    normal = inNormal;

    // convert XYZ vertex to XYZW homogeneous coordinate
    gl_Position = VP * vec4(worldPos, 1.0);
}
