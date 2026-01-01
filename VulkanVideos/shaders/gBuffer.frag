#version 450
#extension GL_EXT_nonuniform_qualifier : require

// ===== Inputs depuis le vertex shader =====
layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

// ===== Outputs G-buffer =====
layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;


layout(set = 1, binding = 0) uniform sampler2D textures[];

layout(set = 2, binding = 0) uniform MaterialData {
    vec4 ambient;          // Ka (Ambient Color) - 16 bytes
    vec4 diffuseColor;     // Kd (Diffuse Color) - 16 bytes
    vec4 specular;         // Ks (Specular Color) - 16 bytes
    vec4 emission;         // Ke (Emission Color) - 16 bytes
    vec4 transmissionFilter; // Tf (Transmission Filter) - 16 bytes

    float shininess;       // Ns (Shininess) - 4 bytes
    float opacity;         // d (Opacity) - 4 bytes
    float transparency;    // Tr (Transparency) - 4 bytes
    float indexOfRefraction; // Ni (IOR) - 4 bytes

    int hasDiffuseMap;     // -1 if no texture, else 1
    int hasNormalMap;
    int hasSpecularMap;
    int hasShininessMap;
    int hasAmbientMap;
    int hasEmissionMap;
    int hasTransmissionMap;
    int illuminationModel; // illum value from MTL

} materialUbo;



void main()
{
    // Debug simple : ROUGE
    //outAlbedo = vec4(1.0, 0.0, 0.0, 1.0);

    outAlbedo = materialUbo.diffuseColor;

    if (materialUbo.hasDiffuseMap != -1) {
        outAlbedo = texture(textures[materialUbo.hasDiffuseMap], fragUV);
    }

    // Normale monde (directe)
    outNormal = vec4(normalize(fragNormalWorld), 1.0);

    // Position monde
    outPosition = vec4(fragPosWorld, 1.0);
}
