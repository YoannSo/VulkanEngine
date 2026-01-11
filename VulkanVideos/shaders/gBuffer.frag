#version 450
#extension GL_EXT_nonuniform_qualifier : require

// ===== Inputs depuis le vertex shader =====
layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;
layout(location = 3) flat in uint matIndex;

// ===== Outputs G-buffer =====
layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;

// ===== Bindless textures =====
layout(set = 1, binding = 0) uniform sampler2D textures[];

// ===== Bindless materials =====
struct MaterialData {
    vec4 ambient;
    vec4 diffuseColor;
    vec4 specular;
    vec4 emission;
    vec4 transmissionFilter;

    vec4 params1; // x = shininess, y = opacity, z = transparency, w = ior
    ivec4 maps1;  // x = diffuse, y = normal, z = specular, w = shininess
    ivec4 maps2;  // x = ambient, y = emission, z = transmission, w = illum
};

layout(set = 2, binding = 0) readonly buffer MaterialBuffer {
    MaterialData materials[];
};

void main()
{
    // Récupère le matériau correspondant
    MaterialData material = materials[matIndex];

    // Couleur diffuse
    outAlbedo = material.diffuseColor;

    // Si un diffuse map existe
    if (material.maps1.x != -1) {
        outAlbedo = texture(textures[material.maps1.x], fragUV);
    }

    // Normale monde
    outNormal = vec4(normalize(fragNormalWorld), 1.0);

    // Position monde
    outPosition = vec4(fragPosWorld, 1.0);
}
