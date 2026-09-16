#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragLuminosity;

in vec2 TexCoord;
in vec4 VertexColor;

uniform sampler2D Texture;
uniform sampler2D TextureLuminosity;
// 0 in full daylight, 1 at full night (see systems::Daytime); gates how strongly light
// sources bloom, so they only visibly glow once it's actually dark.
uniform float night_amount;

void main()
{
    vec4 color = texture(Texture, TexCoord) * VertexColor;
    FragColor = color;

    vec4 luminosity = texture(TextureLuminosity, TexCoord) * VertexColor;
    FragLuminosity = luminosity * night_amount;
}
