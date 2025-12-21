#version 450

// === Global UBO (camera only) ===
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
} ubo;

// === Push constants (par objet) ===
layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

// === Vertex inputs ===
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;   // optionnel
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

// === Outputs vers fragment shader ===
layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormalWorld;
layout(location = 2) out vec2 fragUV;

void main() {
    vec4 worldPos = push.modelMatrix * vec4(inPosition, 1.0);

    gl_Position = ubo.projection * ubo.view * worldPos;

    fragPosWorld = worldPos.xyz;
    fragNormalWorld = normalize(mat3(push.normalMatrix) * inNormal);
    fragUV = inUV;
}
