R""(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertexColor;

uniform sampler2D Texture;

void main()
{
    vec4 color = texture(Texture, TexCoord) * VertexColor;
    FragColor = color;
}
)""
