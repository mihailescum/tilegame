#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragLuminosity;

in vec2 TexCoord;
in vec4 VertexColor;

uniform sampler2D Texture;
uniform sampler2D TextureLuminosity;

void main()
{
    vec4 color = texture(Texture, TexCoord) * VertexColor;
    FragColor = color;

    vec4 luminosity = texture(TextureLuminosity, TexCoord) * VertexColor;
    FragLuminosity = luminosity;
}
