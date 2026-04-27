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


void main()
{
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    gl_Position = globalUbo.lightSpaceMatrix * positionWorld;

}
