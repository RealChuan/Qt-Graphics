#version 440

layout(binding = 1) uniform sampler2D tex;

layout(location = 0) out vec4 fragOutColor;
layout(location = 0) in vec2 fragTexCoord;

void main()
{
    fragOutColor = texture(tex, fragTexCoord);
}

