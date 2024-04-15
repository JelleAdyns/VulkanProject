#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main()
{
    const vec3 lightDirection = normalize(vec3(0.0, 1.0, 1.0));
    float diff = max(dot(fragNormal, lightDirection), 0.2);
    vec3 diffuse = diff*fragColor;
    outColor = vec4(diffuse, 1.0);
}