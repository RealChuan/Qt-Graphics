#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

uniform mat4 transform;

out vec2 TexCord;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCord;

void main()
{
    TexCord = vec2(aTexCord.x, 1.0 - aTexCord.y);
    gl_Position = transform * vec4(aPos, 1.0);
}
