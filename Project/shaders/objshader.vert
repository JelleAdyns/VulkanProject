#version 450

layout(set = 0 , binding = 0)uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} vp;

layout(push_constant)uniform PushConstants
{
    mat4 model;
} mesh;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragTangent;
layout(location = 3) out vec2 fragTexCoord;


void main()
{
    gl_Position = vp.proj * vp.view * mesh.model  * vec4(inPosition, 1.0);
    vec4 tNormal = mesh.model * vec4(inNormal,0);
    fragNormal = normalize(tNormal.xyz);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}