#version 330 core

out vec4 FragColor;    // 输出颜色

in vec2 TexCord;       // 纹理坐标

uniform sampler2D tex; // 纹理

void main()
{
    FragColor = texture(tex, TexCord);
}
