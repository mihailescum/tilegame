#version 330 core
out vec4 FragColor;

//in VERTEX_OUT 
//{
    in vec2 textureCoordinateOut;
    in float faceIndexOut;
//} fs_in;

uniform sampler2D terrainTexture;
uniform sampler1D ambientDiffuseTexture;

void main()
{
    vec4 ambientDiffuseLight = vec4(texture(ambientDiffuseTexture, faceIndexOut));
    FragColor = texture(terrainTexture, textureCoordinateOut) * ambientDiffuseLight;
} 