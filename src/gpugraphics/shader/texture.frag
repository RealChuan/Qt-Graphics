#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

uniform sampler2D tex;

layout(location = 0) out vec4 FragColor;
in vec2 TexCord;

void main()
{
    FragColor = texture(tex, TexCord);
}
