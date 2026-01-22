#version 450

// ===== Global UBO (identique au forward, mais réduit) =====
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
} globalUbo;

// ===== Push constants (identique forward) =====
layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
    uint idMaterial;//x:idMaterial
} push;


// ===== Vertex inputs (MÊMES locations que forward) =====
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;   // pas utilisé mais conservé
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

// ===== Outputs vers fragment shader =====
layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormalWorld;
layout(location = 2) out vec2 fragUV;
layout(location = 3) flat out uint matIndex;
layout(location = 4) out vec3 fragTangentWorld;
layout(location = 5) out vec3 fragBitangentWorld;

void main()
{
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);

    gl_Position = globalUbo.projection * globalUbo.view * positionWorld;

    fragPosWorld    = positionWorld.xyz;
    fragNormalWorld =normalize((push.normalMatrix * vec4(normal, 0.0)).xyz);
    fragTangentWorld =normalize((push.normalMatrix * vec4(tangent, 0.0)).xyz);
    fragBitangentWorld =normalize((push.normalMatrix * vec4(bitangent, 0.0)).xyz);

    fragUV          = uv;
    matIndex=push.idMaterial;
}
