#include <Engine/Managers/MaterialManager.h>


static const std::string MATERIALS_ROOT = "models/basics/materials/";


engine::MaterialManager::MaterialManager(TextureManager& p_textureManager):
	m_materialBuffer{},
	m_textureManager{ p_textureManager }
{
	createBasicMaterial("white",glm::vec4(1.f));
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

uint32_t engine::MaterialManager::createBasicMaterial(const std::string p_name, glm::vec4 p_color) {

	// Guard against exceptions / corrupted map and provide debug info
	try {
		auto [it, inserted] = m_materialNameToID.try_emplace(p_name, m_currentIdMaterial);

		uint32_t id = it->second;

		if (!inserted) {
			if (VERBOSE)
				std::cout << "Material " << p_name << " already exists with ID: " << id << std::endl;
			return id;
		}

		m_materialBuffer.emplace_back(Material(p_name));
		Material& currentMat = m_materialBuffer.back();

		currentMat.setMaterialParameter(Material::EMaterialParameter::DIFFUSECOLOR, p_color);

		++m_currentIdMaterial;

		return id;
	}
	catch (const std::exception& e) {
		// Print diagnostics that will help track down corruption / lifetime problems
		std::cerr << "Exception in createBasicMaterial('" << p_name << "'): " << e.what() << "\n";
		std::cerr << "MaterialManager this=" << this << ", m_materialNameToID size=" << m_materialNameToID.size() << "\n";
		throw;
	}
}

bool engine::MaterialManager::isMaterialAlreadyExisted(int32_t p_materialID) const
{
	if(m_currentIdMaterial > p_materialID && p_materialID >= 0)
		return true;
	return false;
}

void engine::MaterialManager::parseMaterials()
{
	std::string jsonPath = MATERIALS_ROOT + "materials.json";
	std::ifstream file(jsonPath);

	if (!file.is_open()) {
		std::cerr << "Failed to open materials.json at: " << jsonPath << std::endl;
		return;
	}

	nlohmann::json j = nlohmann::json::parse(file);
	for (auto& m : j["materials"]) {
		parseMaterial(m);
	}
}

void engine::MaterialManager::parseMaterial(const nlohmann::json& m)
{
	std::filesystem::path dir = std::filesystem::path(MATERIALS_ROOT);
	std::string folder = m.value("folder", "");
	std::string name = m["name"].get<std::string>();

	// Vérifie si déjà existant (même logique que pour aiMaterial)
	auto [it, inserted] = m_materialNameToID.try_emplace(name, m_currentIdMaterial);
	if (!inserted) {
		std::cout << "Material " << name << " already exists!" << std::endl;
		return;
	}

	m_materialBuffer.emplace_back(Material(name));
	Material& currentMat = m_materialBuffer.back();
	m_currentIdMaterial++;

	// Diffuse color de base
	if (m.contains("color"))
		currentMat.setMaterialParameter(Material::EMaterialParameter::DIFFUSECOLOR,
			glm::vec4(m["color"][0], m["color"][1], m["color"][2], m["color"][3]));

	// Shininess
	if (m.contains("shininess"))
		currentMat.setMaterialParameter(Material::EMaterialParameter::SHININESS,
			m["shininess"].get<float>());

	// Diffuse texture
	if (m.contains("diffuse")) {
		std::string path = (dir / folder / m["diffuse"].get<std::string>()).string();
		uint32_t id = m_textureManager.loadTexture(path);
		currentMat.setMaterialParameter(Material::EMaterialParameter::DIFFUSEMAP, id);
	}

	// Normal texture
	if (m.contains("normal")) {
		std::string path = (dir / folder / m["normal"].get<std::string>()).string();
		uint32_t id = m_textureManager.loadTexture(path);
		currentMat.setMaterialParameter(Material::EMaterialParameter::NORMALMAP, id);
	}

	// Specular texture
	if (m.contains("specular")) {
		std::string path = (dir / folder / m["specular"].get<std::string>()).string();
		uint32_t id = m_textureManager.loadTexture(path);
		currentMat.setMaterialParameter(Material::EMaterialParameter::SPECULARMAP, id);
	}
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

