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
// True for the opaque (depth-tested) pass - tiles and characters, drawn with alpha blending
// off - so a texture's fully-transparent regions are discarded instead of painting solid
// garbage into the color/depth buffers. False for the particle pass, which blends real partial
// alpha instead of a hard cutoff.
uniform bool discard_transparent;

void main()
{
    vec4 color = texture(Texture, TexCoord) * VertexColor;
    if (discard_transparent && color.a < 0.5)
    {
        discard;
    }
    FragColor = color;

    vec4 luminosity = texture(TextureLuminosity, TexCoord) * VertexColor;
    FragLuminosity = luminosity * night_amount;
}
