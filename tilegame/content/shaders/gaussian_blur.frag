#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D scene;
// (1, 0) for a horizontal pass, (0, 1) for a vertical pass - each direction is its own
// PostProcessingEffect/Shader instance (see systems::Render::load_content()), chained so the
// second consumes the first's output, since a proper (separable) Gaussian blur is two 1D
// passes rather than one 2D pass.
uniform vec2 direction;
// 1.0 / render target resolution, so sample offsets are independent of viewport size.
uniform vec2 texel_size;

void main()
{
    const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    vec3 result = texture(scene, TexCoord).rgb * weights[0];
    for (int i = 1; i < 5; ++i)
    {
        vec2 offset = direction * texel_size * float(i);
        result += texture(scene, TexCoord + offset).rgb * weights[i];
        result += texture(scene, TexCoord - offset).rgb * weights[i];
    }

    FragColor = vec4(result, 1.0);
}
