#version 450

layout(binding = 1) uniform sampler2D diffuseTexSampler;

layout(push_constant)uniform RenderModes
{
    layout(offset = 64) uint renderingMode;
    bool useNormalMap;
} renderProperties;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragTangent;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    const vec3 lightDirection = vec3(.577f, -.577f, .577f);
    float observedArea = max(dot(fragNormal, -lightDirection), 0.2);
    vec3 diffuse = observedArea*fragColor;
    
    if(renderProperties.renderingMode == 2) outColor = vec4(diffuse, 1.0);
    else outColor = vec4(diffuse, 1.0) * texture(diffuseTexSampler, fragTexCoord);
}