#version 330 core

uniform mat4 transform;

in vec3 aPos;
in vec2 aTexCord;

out vec2 TexCord; // 纹理坐标

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    TexCord = vec2(aTexCord.x, 1.0 - aTexCord.y);
}
