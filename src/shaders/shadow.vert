#version 450

// ===== Global UBO (identique au forward, mais réduit) =====
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 lightSpaceMatrix;
} globalUbo;

// ===== Push constants (identique forward) =====
layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
    uint idMaterial;//x:idMaterial
} push;


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;   // pas utilisé mais conservé
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;



void main()
{
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    gl_Position = globalUbo.lightSpaceMatrix * positionWorld;

}
