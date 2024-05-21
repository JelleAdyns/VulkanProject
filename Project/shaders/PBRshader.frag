#version 450


layout(binding = 1) uniform sampler2D diffuseTexSampler;
layout(binding = 2) uniform sampler2D normalTexSampler;
layout(binding = 3) uniform sampler2D roughnessTexSampler;
layout(binding = 4) uniform sampler2D specularTexSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragTangent;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in vec4 fragWorldPos;
layout(location = 5) in vec3 fragCameraPos;

layout(location = 0) out vec4 outColor;

void main()
{
    //------------------------------------------------------------------------------------------------------------------------------------------------
    // CONSTANTS

    const vec3 lightDirection = normalize(vec3(0.577, -0.577, 0.577));
    const float lightIntensity = 5.f;
    const float shininess = 25.f;
    const vec3 ambient = { 0.01f, 0.01f, 0.01f };
    const float PI = 3.14159265358979323846f;

    //------------------------------------------------------------------------------------------------------------------------------------------------



    //------------------------------------------------------------------------------------------------------------------------------------------------
    // NORMAL CALCULATIONS


    //Shoutout to this guy for the normal map format!!
    //https://www.reddit.com/r/vulkan/comments/wksa4z/strange_issue_with_normal_maps_in_pbr_shader/


    vec3 N = normalize(fragNormal);
    vec3 T = normalize(fragTangent);
    vec3 B = normalize(cross(N, T));

    mat3 tangentSpaceAxis = mat3(T, B, N);
  
    vec3 normalSample = texture(normalTexSampler, fragTexCoord).rgb * 2.0 - 1.0;


    normalSample = tangentSpaceAxis * normalize(normalSample );
    //normalSample = vec3(normalSample.x,-normalSample.y,normalSample.z);

 
    float observedArea = dot(normalSample, -lightDirection);
    //float observedArea = dot(fragNormal, -lightDirection); //CORRECT!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //------------------------------------------------------------------------------------------------------------------------------------------------


    if (observedArea > 0) 
    {
        //------------------------------------------------------------------------------------------------------------------------------------------------
        // DIFFUSE CALCUATIONS

        vec3 diffuse = (lightIntensity *  texture(diffuseTexSampler, fragTexCoord).rgb / PI) * observedArea; //CORRECT!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        //------------------------------------------------------------------------------------------------------------------------------------------------


        //------------------------------------------------------------------------------------------------------------------------------------------------
        // GLOSS CALCUATIONS

        vec3 reflection = reflect(lightDirection, normalSample);
        //vec3 reflection = reflect(lightDirection, fragNormal); //CORRECT!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        vec3 invViewDirection = normalize(fragCameraPos - fragWorldPos.xyz );

        const float cosAlpha = max(0.f, dot(reflection,invViewDirection));
        const vec3 specular = pow(cosAlpha, texture(roughnessTexSampler, fragTexCoord).r * shininess) * texture(specularTexSampler, fragTexCoord).rgb;

        //------------------------------------------------------------------------------------------------------------------------------------------------


        //------------------------------------------------------------------------------------------------------------------------------------------------
        // OUPUT

        //outColor = clamp(vec4(diffuse ,0.0),0.0,1.0); //CORRECT!!!!!!!!!!!!!!!!!!!
        //outColor = clamp(vec4(specular ,0.0),0.0,1.0);
        outColor = clamp(vec4(diffuse  + specular + ambient,1.0),0.0,1.0);
        //outColor = clamp(vec4(observedArea,observedArea,observedArea, 1.0),0.0,1.0);

        //------------------------------------------------------------------------------------------------------------------------------------------------
    }
    else
    {
        //outColor = vec4(vec3(0.0, 0.0, 0.0), 1.0);
        outColor = vec4(ambient, 0.0f);
    }
}