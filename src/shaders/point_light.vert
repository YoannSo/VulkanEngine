#version 450
//vert

//use to create billboard object, offsetting each vertex pos from the point light pos
const vec2 OFFSETS[6] = vec2[]( 
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);


layout(location=0) out vec2 fragOffset;

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
}ubo;

layout(push_constant) uniform Push{
	vec4 position;
	vec4 color;
	float radius;	
}push;



void main(){
	fragOffset=OFFSETS[gl_VertexIndex];

	vec4 ligthInCameraSpace=ubo.view*push.position;
	vec4 positionInCameraSpace=ligthInCameraSpace+push.radius*vec4(fragOffset,0.0,0.0);

	gl_Position=ubo.projection*positionInCameraSpace;
}


