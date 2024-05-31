#version 330 core

uniform sampler2D tex; // 纹理

in vec2 TexCord; // 纹理坐标

out vec4 FragColor; // 输出颜色

void main()
{
    FragColor = texture(tex, TexCord);
}
