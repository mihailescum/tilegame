#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D scene;
uniform sampler2D daytime_tint;

uniform float time;
uniform vec4 weather_tint;
uniform float flash_intensity;

void main()
{
    vec4 color = texture(scene, TexCoord) * weather_tint * texture(daytime_tint, vec2(time, 0.25));
    color = mix(color, vec4(1.0), flash_intensity);
    FragColor = color;
}
