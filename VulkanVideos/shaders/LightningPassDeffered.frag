#version 450

struct PointLight{
	vec4 position; // ingore w for aligment
	vec4 color; //w = intenity 
};

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 fragUV;


// G-buffer
layout(set = 1, binding = 0) uniform sampler2D gAlbedo;
layout(set = 1, binding = 1) uniform sampler2D gNormal;
layout(set = 1, binding = 2) uniform sampler2D gPosition;

// Global
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 inverseView;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

const vec3 specularColor = vec3(1.0);
const float shininess = 32.0;

void main()
{
    if(ubo.numLights==0)

        outColor = vec4(1.0,1.0,1.0, 1.0);
        else
    outColor = vec4(0.0,0.0,1.0, 1.0);

}
