#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCord;
out vec2 TexCord; // 纹理坐标

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    TexCord = vec2(aTexCord.x, 1.0 - aTexCord.y);
}
