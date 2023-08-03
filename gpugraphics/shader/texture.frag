#version 450

layout(binding = 0) uniform sampler2D tex; // 纹理

layout(location = 0) in vec2 TexCord; // 纹理坐标

layout(location = 0) out vec4 FragColor; // 输出颜色

void main()
{
    FragColor = texture(tex, TexCord);
}
