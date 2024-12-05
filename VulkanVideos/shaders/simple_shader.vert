#version 450
//vert

struct PointLight{
	vec4 position; // ingore w for aligment
	vec4 color; //w = intenity 
};

layout(set=0, binding=0)uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 inverseView;
	vec4 ambientLightColor;
	PointLight pointLights[10];//10 can be specilization constant
	int numLights;
}globaUbo;


layout(set=2, binding=0)uniform ObjectUbo{
	int _idText;
}objectUbo;

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;	
}push;



layout(location = 0) in vec3 position;//text value from a vertex buffer
layout(location=1) in vec3 color;
layout(location=2) in vec3 normal;
layout(location=3) in vec2 uv;

// no association between input loc and output loc
layout(location=0) out vec3 fragColor;
layout(location=1) out vec3 fragPosWorld;
layout(location=2) out vec3 fragNormalWorld;
layout(location=3) out vec2 uvOut;


void main(){//once for each vertex
  vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
  gl_Position = globaUbo.projection * globaUbo.view * positionWorld;
  fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
  fragPosWorld = positionWorld.xyz;
  fragColor = vec3(1.0,0.0,0.0);
  uvOut=uv;
}