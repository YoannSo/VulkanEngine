#version 450
//frag

layout (location=0) out vec4 outColor;
//layout => location value,  multiple location, now only locatio0, ut => output

layout (location=0)in vec3 inColor;
layout(location=1) in vec3 inFragPosWorld;
layout(location=2) in vec3 inFragNormalWorld;
layout(location=3) in vec2 inFragUV;

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


layout(set = 1, binding = 0) uniform sampler2D image;


layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;	
}push;




void main(){


	vec3 diffuseLight=ubo.ambientLightColor.xyz*ubo.ambientLightColor.w;
	vec3 specularLight=vec3(0.f);

	vec3 surfaceNormal=normalize(inFragNormalWorld);

	vec3 cameraPosWorld=ubo.inverseView[3].xyz;
	vec3 viewDir=normalize(cameraPosWorld-inFragPosWorld);


	for(int i=0;i<ubo.numLights;i++){
		PointLight currentLight=ubo.pointLights[i];

		vec3 directionToLight=currentLight.position.xyz-inFragPosWorld;
		float attenuation=1.0/ dot(directionToLight,directionToLight);

		directionToLight=normalize(directionToLight);


		float cosAngIncidence=max(dot(surfaceNormal,directionToLight),0.f);
		vec3 intensity=currentLight.color.xyz* currentLight.color.w*attenuation;



		diffuseLight+=intensity*cosAngIncidence;


		//specular part

		vec3 halfAngle=normalize(directionToLight+viewDir);
		float blinnTerm=dot(surfaceNormal,halfAngle);

		blinnTerm=clamp(blinnTerm,0,1);
		blinnTerm=pow(blinnTerm,32.f);//higher exponnet => shaper highlight

		//specularLight+= currentLight.color.xyz*attenuation*blinnTerm;
		specularLight+= intensity*blinnTerm;

	}

	PointLight cameraLight= PointLight(vec4(cameraPosWorld,1.0f),vec4(1.0f));

		vec3 directionToLight=cameraLight.position.xyz-inFragPosWorld;
		float attenuation=1.0/ dot(directionToLight,directionToLight);

		directionToLight=normalize(directionToLight);


		float cosAngIncidence=max(dot(surfaceNormal,directionToLight),0.f);
		vec3 intensity=cameraLight.color.xyz* cameraLight.color.w*attenuation;



		diffuseLight+=intensity*cosAngIncidence;


		//specular part

		vec3 halfAngle=normalize(directionToLight+viewDir);
		float blinnTerm=dot(surfaceNormal,halfAngle);

		blinnTerm=clamp(blinnTerm,0,1);
		blinnTerm=pow(blinnTerm,32.f);//higher exponnet => shaper highlight

		//specularLight+= currentLight.color.xyz*attenuation*blinnTerm;
		specularLight+= intensity*blinnTerm;



  vec3 imageColor = texture(image, inFragUV).rgb;

	outColor=vec4((diffuseLight*inColor+specularLight*inColor)*imageColor,1.0);
}