#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D scene;
uniform vec4 tint_color;

void main()
{
    vec4 color = texture(scene, TexCoord) * tint_color;
    FragColor = color;
}
