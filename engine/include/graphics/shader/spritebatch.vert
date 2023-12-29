R""(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

out vec2 TexCoord;
out vec4 VertexColor;

uniform mat4 WVP;

void main()
{
    gl_Position = WVP * vec4(position.xyz, 1.0);
    TexCoord = uv;
    VertexColor = color;
}
)""