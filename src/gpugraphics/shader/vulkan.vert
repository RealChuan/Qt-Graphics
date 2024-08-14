#version 450

layout(binding = 0, std140) uniform UniformBufferObject
{
    mat4 transform;
} ubo;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) in vec2 inTexCoord;
layout(location = 0) in vec4 inPosition;

void main()
{
    fragTexCoord = inTexCoord;
    gl_Position = ubo.transform * inPosition;
}
