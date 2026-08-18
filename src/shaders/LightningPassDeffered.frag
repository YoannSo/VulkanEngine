#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

// ===== G-buffer =====
layout(set = 1, binding = 0) uniform sampler2D gAlbedo;
layout(set = 1, binding = 1) uniform sampler2D gSpecular;
layout(set = 1, binding = 2) uniform sampler2D gNormal;
layout(set = 1, binding = 3) uniform sampler2D gPosition;

// ===== Global UBO (ordre doit matcher SceneManager::GlobalUbo en C++) =====
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 lightSpace;
    mat4 inverseView;
};

// ===== Lights =====
struct LightGPU {
    vec4 position;   // xyz = position, w = type
    vec4 direction;  // xyz = direction
    vec4 color;      // rgb = color, w = intensity
    vec4 params;     // x = type
};

layout(set = 2, binding = 0) readonly buffer Lights {
    LightGPU lights[];
} lightBuffer;

// ===== Shadow map =====
layout(set = 3, binding = 0) uniform sampler2D shadowMap;

layout(push_constant) uniform LightPC {
    uint lightCount;
    uint showNormal;    // 1: yes, 0: no
    uint showLighning;  // 1: yes, 0: no
} pushConstant;

// Retourne une valeur entre 0.0 (pleine lumière) et 1.0 (pleine ombre)
float isInShadow(vec3 fragPos, vec3 normal, vec3 lightDir)
{
    vec4 fragPosLightSpace = lightSpace * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5; // NDC -> UV [0,1]

    // Hors du frustum de la lumière -> pas d'ombre
    if (projCoords.z > 1.0 || projCoords.z < 0.0
        || projCoords.x < 0.0 || projCoords.x > 1.0
        || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float currentDepth = projCoords.z - bias;

    // PCF: on moyenne sur un voisinage de 3x3 texels pour adoucir les bords
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{

    vec3 albedo = texture(gAlbedo, fragUV).rgb;
    vec3 normal = normalize(texture(gNormal, fragUV).xyz);
    vec3 specular = texture(gSpecular, fragUV).rgb;
    vec3 fragPos = texture(gPosition, fragUV).xyz;
    vec3 cameraPos = inverseView[3].xyz;
    vec3 viewDir = normalize(cameraPos - fragPos);

    vec3 color = vec3(0.0);

    if (pushConstant.showLighning == 1) {
        color = albedo * vec3(0.12); // ambient

        for (uint i = 0; i < pushConstant.lightCount; i++) {
            LightGPU light = lightBuffer.lights[i];
            uint type = uint(light.position.w);

            if (type == 1) { // Directionnel
                vec3 L = normalize(-light.direction.xyz);

                float shadow = isInShadow(fragPos, normal, L);
                float NdotL = max(dot(normal, L), 0.0);

                // Blinn-Phong
                vec3 H = normalize(L + viewDir);
                float spec = pow(max(dot(normal, H), 0.0), 16.0);

                color += albedo * light.color.rgb * light.color.w * NdotL * (1.0 - shadow);
                color += light.color.rgb * light.color.w * spec * 0.5 * (1.0 - shadow);
            }
            else if (type == 0) { // Point light
                /*vec3 L = light.position.xyz - fragPos;
                float distance = length(L);
                L = normalize(L);

                float NdotL = max(dot(normal, L), 0.0);
                float attenuation = 1.0 / (distance * distance + 1.0);

                // Blinn-Phong
                vec3 H = normalize(L + viewDir);
                float spec = pow(max(dot(normal, H), 0.0), 16.0)* specular.r;

                color += albedo * light.color.rgb * light.color.w * NdotL * attenuation;
                color += light.color.rgb * light.color.w * spec * 0.5 * attenuation;*/
            }
        }
    }
    else {
        color = albedo;
    }

    if (pushConstant.showNormal == 1)
        outColor = vec4(normal, 1.0);
    else
        outColor = vec4(color, 1.0);
}