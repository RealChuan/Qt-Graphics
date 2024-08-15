#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

uniform mat4 transform;

out vec2 fragTexCoord;
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

void main()
{
    fragTexCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
    gl_Position = transform * vec4(inPosition, 1.0);
}
