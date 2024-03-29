#version 330 core

layout(location = 0) in vec4 vertex; //vec2 position, vec2 uv

out vec2 TexCoord;

uniform mat4 WVP;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    TexCoord = vertex.zw;
}
