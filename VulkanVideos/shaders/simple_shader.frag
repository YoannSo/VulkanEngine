#version 450
#extension GL_EXT_nonuniform_qualifier : require


//frag

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

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;	
}push;


layout (location=0)in vec3 inColor;
layout(location=1) in vec3 inFragPosWorld;
layout(location=2) in vec3 inFragNormalWorld;
layout(location=3) in vec2 inUV;

layout (location=0) out vec4 outColor;

//layout => location value,  multiple location, now only locatio0, ut => output

void main(){
	if(materialUbo.opacity<0.5){
    outColor = vec4(materialUbo.diffuseColor.xyz,0.5);
	return;
	}
    vec3 diffuseColor = materialUbo.diffuseColor.xyz;
    if (materialUbo.hasDiffuseMap != -1) {
        diffuseColor = texture(textures[materialUbo.hasDiffuseMap], inUV).xyz;
    }

    vec3 ambientColor = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    if (materialUbo.hasAmbientMap != -1) {
        ambientColor = texture(textures[materialUbo.hasAmbientMap], inUV).xyz;
    }

    vec3 specularColor = materialUbo.specular.xyz;
    if (materialUbo.hasSpecularMap != -1) {
        specularColor = texture(textures[materialUbo.hasSpecularMap], inUV).xyz;
    }

    vec3 diffuseLight = ambientColor;
    vec3 specularLight = vec3(0.0);


	vec3 surfaceNormal=normalize(inFragNormalWorld);

	vec3 cameraPosWorld=ubo.inverseView[3].xyz;


	/*for(int i=0;i<ubo.numLights;i++){
		PointLight currentLight=ubo.pointLights[i];

		vec3 directionToLight=currentLight.position.xyz-inFragPosWorld;
		float attenuation=1.0/ dot(directionToLight,directionToLight);

		directionToLight=normalize(directionToLight);


		float cosAngIncidence=max(dot(surfaceNormal,directionToLight),0.f);
		vec3 intensity=currentLight.color.xyz* currentLight.color.w*attenuation;



		diffuseLight+=1*diffuseColor;


		//specular part

		vec3 halfAngle=normalize(directionToLight+viewDir);
		float blinnTerm=dot(surfaceNormal,halfAngle);

		blinnTerm=clamp(blinnTerm,0,1);
		blinnTerm=pow(blinnTerm,32.f);//higher exponnet => shaper highlight

		//specularLight+= currentLight.color.xyz*attenuation*blinnTerm;
		specularLight+= intensity*blinnTerm*specularColor;

	}*/
	
	//outColor=vec4(inUV,0.0,1.0);
	//if(objectUbo.idText!=1)
   		//outColor = texture(textures[objectUbo.idText], inUV);
	//else
		//outColor=vec4((diffuseLight*inColor+specularLight*inColor),1.0);
	//	if(materialUbo.hasSpecularMap!=-1)
	//		outColor = texture(textures[materialUbo.hasSpecularMap], inUV);
	//	else
	//		outColor=vec4(materialUbo.diffuseColor.xyz,1.0);
	vec3 viewDir=normalize(cameraPosWorld-inFragPosWorld);

	float dotProduct = dot(surfaceNormal, viewDir);
	dotProduct-=0.5;

    outColor = vec4(dotProduct,0.0,0.0, 1.0);


}