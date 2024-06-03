#version 450

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 transform;
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    fragTexCoord = inTexCoord;
    gl_Position = ubo.transform * inPosition;
}
