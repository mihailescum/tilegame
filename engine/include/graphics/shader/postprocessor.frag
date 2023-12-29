R""(
    #version 330 core
    in  vec2  TexCoords;
    out vec4  color;
    
    uniform sampler2D scene;

    void main()
    {
        color = vec4(0.0f);
        color = texture(scene, TexCoords) * 0.5;
    }
)""