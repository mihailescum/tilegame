#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in float faceIndex;

//out VERTEX_OUT 
//{
    out vec2 textureCoordinateOut;
    out float faceIndexOut;
//} vs_out;

uniform mat4 viewProjection;

void main()
{
    gl_Position = viewProjection * vec4(position.xyz, 1.0);
    textureCoordinateOut = textureCoordinate;
    faceIndexOut = faceIndex;
}