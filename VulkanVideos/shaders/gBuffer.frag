#version 450

// === Inputs depuis le vertex shader ===
layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUV;

// === Outputs G-buffer ===
// Attachment 0 : Albedo
layout(location = 0) out vec4 outAlbedo;

// Attachment 1 : Normal (encodée 0–1)
layout(location = 1) out vec4 outNormal;

// Attachment 2 : Position (ou autre data)
layout(location = 2) out vec4 outPosition;

// === Matériau simple (pour l’instant) ===
layout(set = 2, binding = 0) uniform ObjectUbo {
    int textureId; // tu pourras brancher une texture plus tard
} objectUbo;

void main() {
    // Albedo fixe pour debug (rouge)
    outAlbedo = vec4(1.0, 0.0, 0.0, 1.0);

    // Normale encodée en [0,1]
    vec3 normal = normalize(fragNormalWorld);
    outNormal = vec4(normal * 0.5 + 0.5, 1.0);

    // Position monde
    outPosition = vec4(fragPosWorld, 1.0);
}
