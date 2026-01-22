#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

// ===== G-buffer =====
layout(set = 1, binding = 0) uniform sampler2D gAlbedo;
layout(set = 1, binding = 1) uniform sampler2D gNormal;
layout(set = 1, binding = 2) uniform sampler2D gPosition;

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
} pushConstant;

void main()
{
    vec3 albedo = texture(gAlbedo, fragUV).rgb;
    vec3 normal = normalize(texture(gNormal, fragUV).xyz);

    // Hard-coded ambient (debug)
    const vec3 ambientColor = vec3(0.9); // increase to brighten overall scene
    vec3 color = albedo * ambientColor;

    for (uint i = 0; i < pushConstant.lightCount; i++) {
        LightGPU light = lightBuffer.lights[i];
        uint type = uint(light.position.w);
        if (type == 1) {
            vec3 L = normalize(-light.direction.xyz);
            float NdotL = max(dot(normal, L), 0.0);
            color += albedo * light.color.rgb * light.color.w * NdotL;
        }
        else if (type == 0) {
            // World-space fragment position
            vec3 fragPos = texture(gPosition, fragUV).xyz;

            // Vector to light
            vec3 L = light.position.xyz - fragPos;

            float distance = length(L);
            L = normalize(L);

            // Diffuse Lambert
            float NdotL = max(dot(normal, L), 0.0);

            // Simple attenuation
            float attenuation = 1.0 / (distance * distance + 1.0);

            // Contribution
            color += albedo
                   * light.color.rgb
                   * light.color.w
                   * NdotL
                   * attenuation;
        }
    }

    outColor = vec4(color, 1.0);
}