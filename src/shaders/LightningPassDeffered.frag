#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

// ===== G-buffer =====
layout(set = 1, binding = 0) uniform sampler2D gAlbedo;
layout(set = 1, binding =1) uniform sampler2D gSpecular;
layout(set = 1, binding = 2) uniform sampler2D gNormal;
layout(set = 1, binding = 3) uniform sampler2D gPosition;

// lighting.frag
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
};



// ===== Lights =====
struct LightGPU {
    vec4 position;   // xyz = position, w = type
    vec4 direction;  // xyz = direction
    vec4 color;      // rgb = color, w = intensity
    vec4 params;     // x=type
};

layout(set = 2, binding = 0) readonly buffer Lights {
    LightGPU lights[];
} lightBuffer;


layout(push_constant) uniform LightPC {
    uint lightCount;
    uint showNormal; //1:yes 0:no
    uint showLighning; //1:yes 0:no
} pushConstant;

void main()
{
    vec3 albedo = texture(gAlbedo, fragUV).rgb;
    vec3 normal = normalize(texture(gNormal, fragUV).xyz);
    vec3 specular = texture(gSpecular, fragUV).rgb;
    vec3 fragPos = texture(gPosition, fragUV).xyz;
    vec3 cameraPos = inverseView[3].xyz;
    vec3 viewDir = normalize(cameraPos - fragPos);

    vec3 color = vec3(0.0);

    if(pushConstant.showLighning == 1) {
        color = albedo * vec3(0.12); // ambient

        for (uint i = 0; i < pushConstant.lightCount; i++) {
            LightGPU light = lightBuffer.lights[i];
            uint type = uint(light.position.w);

            if (type == 1) { // Directionnal
                vec3 L = normalize(-light.direction.xyz);
                float NdotL = max(dot(normal, L), 0.0);

                // Blinn-Phong
                vec3 H = normalize(L + viewDir);
                float spec = pow(max(dot(normal, H), 0.0), 16.0);

                color += albedo * light.color.rgb * light.color.w * NdotL;
                color += light.color.rgb * light.color.w * spec * 0.5;
            }
            else if (type == 0) { // Point light
                vec3 L = light.position.xyz - fragPos;
                float distance = length(L);
                L = normalize(L);

                float NdotL = max(dot(normal, L), 0.0);
                float attenuation = 1.0 / (distance * distance + 1.0);

                // Blinn-Phong
                vec3 H = normalize(L + viewDir);
                float spec = pow(max(dot(normal, H), 0.0), 16.0)* specular.r;

                color += albedo * light.color.rgb * light.color.w * NdotL * attenuation;
                color += light.color.rgb * light.color.w * spec * 0.5 * attenuation;
            }
        }
    }
    else {
        color = albedo;
    }

    if(pushConstant.showNormal == 1)
        outColor = vec4(normal, 1.0);
    else
        outColor = vec4(color, 1.0);
}