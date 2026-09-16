#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D scene;
uniform vec4 tint_color;
uniform vec4 weather_tint;
uniform float flash_intensity;

void main()
{
    vec4 color = texture(scene, TexCoord) * tint_color * weather_tint;
    color = mix(color, vec4(1.0), flash_intensity);
    FragColor = color;
}
