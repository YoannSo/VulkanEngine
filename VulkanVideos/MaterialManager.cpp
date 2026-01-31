#include "MaterialManager.h"

engine::MaterialManager::MaterialManager(TextureManager& p_textureManager):
	m_materialBuffer{},
	m_textureManager{ p_textureManager }
{
	createBasicMaterial();
}

engine::MaterialManager::~MaterialManager()
{
}

uint32_t engine::MaterialManager::createMaterialsFromMesh(const aiScene& p_scene,std::string p_name)
{
	_dirPath = "models/" + p_name + "/";
	uint32_t materialCount = p_scene.mNumMaterials;
	for (uint32_t i = 0; i < materialCount; ++i) {
		createMaterial(p_scene.mMaterials[i]);
		++m_currentIdMaterial;
	}
	return materialCount;


}

void engine::MaterialManager::fillMaterialsBufferData(std::vector<Material::MaterialGPU>& p_materialsBufferData) const
{
	p_materialsBufferData.reserve(m_materialBuffer.size());
	for (const auto& materialPair : m_materialBuffer) {
		p_materialsBufferData.emplace_back(materialPair.getMaterialBuffer());
	}
}

uint32_t engine::MaterialManager::getIDOfMaterial(const std::string& p_materialName) const
{
	auto it = m_materialNameToID.find(p_materialName);
	if (it != m_materialNameToID.end()) {
		return it->second;
	}
	else {
		if (VERBOSE)
			std::cout << "Material " << p_materialName << " not found!" << std::endl;
		return 0;
	}
}

void engine::MaterialManager::createBasicMaterial() {
	auto [it, inserted] = m_materialNameToID.try_emplace("basic", 0);

	m_materialBuffer.emplace_back(Material("basic"));
	Material& currentMat = m_materialBuffer.back();

	currentMat.setMaterialParameter(Material::EMaterialParameter::DIFFUSECOLOR, glm::vec4(1.f, 0.f, 0.f, 1.f));
	++m_currentIdMaterial;

}

void engine::MaterialManager::createMaterial(const aiMaterial* p_mtl)
{
	aiString materialName;

	p_mtl->Get(AI_MATKEY_NAME, materialName);


	auto [it, inserted] = m_materialNameToID.try_emplace(materialName.C_Str(),m_currentIdMaterial);

	m_materialBuffer.emplace_back(Material(materialName.C_Str()));

	Material& currentMat = m_materialBuffer.back();

	if (!inserted) {
		if (VERBOSE)
			std::cout << "Material " << materialName.C_Str() << " already exists!" << std::endl;
		return;
	}

	aiColor3D color;
	aiString  texturePath;

	// ===================================================== AMBIENT
	if (p_mtl->GetTextureCount(aiTextureType_AMBIENT) > 0) // Texture ?
	{
		p_mtl->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);
		std::string completePath = _dirPath + texturePath.C_Str();

		uint32_t id=m_textureManager.loadTexture(completePath);

		if (VERBOSE)
			std::cout << "-*- Load Ambient Map" << completePath << " at ID : " << id << std::endl;

		currentMat.setMaterialParameter(Material::EMaterialParameter::AMBIENTMAP, id);
	}
	else if (p_mtl->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) // else Material ?
	{
		//material._ambient = glm::vec3(color.r, color.g, color.b);
	}

	// ===================================================== NORMAL
	if (p_mtl->GetTextureCount(aiTextureType_NORMALS) > 0) // Texture ?
	{

		p_mtl->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
		std::string completePath = _dirPath + texturePath.C_Str();

		uint32_t id = m_textureManager.loadTexture(completePath);

		if (VERBOSE)
			std::cout << "-*- Load Normal Map" << completePath << " at ID : " << id << std::endl;

		currentMat.setMaterialParameter(Material::EMaterialParameter::NORMALMAP, id);

	}
	else if (p_mtl->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) // else Material ?
	{
		//material._ambient = glm::vec3(color.r, color.g, color.b);
	}

	// ===================================================== DIFFUSE
	if (p_mtl->GetTextureCount(aiTextureType_DIFFUSE) > 0) // Texture ?
	{
		p_mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
		std::string completePath = _dirPath + texturePath.C_Str();

		uint32_t id = m_textureManager.loadTexture(completePath);

		if (VERBOSE)
			std::cout << "-*- Load Diffuse Map" << completePath << " at ID : " << id << std::endl;

		currentMat.setMaterialParameter(Material::EMaterialParameter::DIFFUSEMAP, id);
	}
	else if (p_mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) // else Material ?
	{
		if (VERBOSE)
			std::cout << "DIFFUSE COLOR " << color.r << color.b << color.g << std::endl;
		currentMat.setMaterialParameter(Material::EMaterialParameter::DIFFUSECOLOR, glm::vec4(color.r, color.g, color.b, 0.f));
	}




	// ===================================================== SPECULAR
	if (p_mtl->GetTextureCount(aiTextureType_SPECULAR) > 0) // Texture ?
	{
		p_mtl->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
		std::string completePath = _dirPath + texturePath.C_Str();

		uint32_t id = m_textureManager.loadTexture(completePath);

		if (VERBOSE)
			std::cout << "-*- Load Specular Map" << completePath << " at ID : " << id << std::endl;

		currentMat.setMaterialParameter(Material::EMaterialParameter::SPECULARMAP, id);
	}
	else if (p_mtl->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) // else Material ?
	{
		currentMat.setMaterialParameter(Material::EMaterialParameter::SPECULARCOLOR, glm::vec4(color.r, color.g, color.b, 0.f));
	}
	// =====================================================

	// ===================================================== SHININESS
	float shininess;
	if (p_mtl->GetTextureCount(aiTextureType_SHININESS) > 0) // Texture ?
	{
		p_mtl->GetTexture(aiTextureType_SHININESS, 0, &texturePath);

		//material._shininessMap = std::make_shared<Texture>( _dirPath + texturePath.C_Str());
		//Model::s_allTexturesName.emplace_back(_dirPath + texturePath.C_Str());
		//material._hasShininessMap = true;
	}
	else if (p_mtl->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) // else Material ?
	{
		currentMat.setMaterialParameter(Material::EMaterialParameter::SHININESS, shininess);
	}

	float opacity;
	if (p_mtl->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
	{
		currentMat.setMaterialParameter(Material::EMaterialParameter::OPACITY, opacity);
	}

	float indexOfRefraction;
	if (p_mtl->Get(AI_MATKEY_REFRACTI, indexOfRefraction) == AI_SUCCESS)
	{
		currentMat.setMaterialParameter(Material::EMaterialParameter::REFRACTION, indexOfRefraction);
	}

	float transparency;
	if (p_mtl->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparency) == AI_SUCCESS)
	{
		currentMat.setMaterialParameter(Material::EMaterialParameter::TRANSPARENCY, transparency);
	}

	float illuminationModel;
	if (p_mtl->Get(AI_MATKEY_SHADING_MODEL, illuminationModel) == AI_SUCCESS)
	{
		currentMat.setMaterialParameter(Material::EMaterialParameter::ILLUMINATIONMODEL, illuminationModel);
	}

	//material.setupDescriptorSet();    <-------------------------------- IMPRTANT
}
