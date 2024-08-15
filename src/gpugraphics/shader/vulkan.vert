#version 440

layout(binding = 0, std140) uniform UniformBufferObject
{
    mat4 transform;
}ubo;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

void main()
{
    fragTexCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
    gl_Position = ubo.transform * vec4(inPosition, 1.0);
}
