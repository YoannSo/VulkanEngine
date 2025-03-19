#include "model.hpp"

#include "SceneManager.h"

namespace lve {
	

	Model::Model(const std::string p_name, const std::string p_filePath) :GameObject(), _name{ p_name }, _filePath{ p_filePath }
	{
		std::cout << "Loading model " << _name << " from: " << p_filePath << std::endl;



		Assimp::Importer importer;
		_dirPath = "models/" + _name + "/";

		const unsigned int flags = aiProcessPreset_TargetRealtime_Fast |aiProcess_Triangulate| aiProcess_FlipUVs| aiProcess_JoinIdenticalVertices;

		const aiScene* const scene = importer.ReadFile(p_filePath, flags);

		if (scene == nullptr)
		{
			std::cout << "FAILED model " << _name << " from: " << p_filePath<< importer.GetErrorString() << std::endl;

			throw std::runtime_error("Fail to load file \" " + p_filePath + "\": " + importer.GetErrorString());
		}

		m_nbMaterials = scene->mNumMaterials;

			for (uint32_t i = 0; i < m_nbMaterials; ++i) {
				SceneManager::getInstance()->addMaterial(std::make_unique<Material>(_loadMaterial(scene->mMaterials[i])));
			}

		_nbMeshes = scene->mNumMeshes;
		std::cout <<  "--** Your model got " <<  _nbMeshes << " meshes **--" << std::endl;

		for (uint32_t i = 0; i < _nbMeshes; ++i) {
			_loadMesh(scene->mMeshes[i], scene);
		}


	}
	Model::~Model()
	{
	}

	std::vector<std::string> Model::LoadMaterials(const aiScene* p_scene)
	{
		return std::vector<std::string>();
	}
	std::vector<TriangleMesh*>  Model::getAllMeshesFromMaterial(const std::string p_materialName) {
		auto range = m_meshes.equal_range(p_materialName);

		std::vector<TriangleMesh*> returnList{};

		for (auto it = range.first; it != range.second; ++it) {
			returnList.push_back(it->second);
		}

		return returnList;

	}
	void Model::_loadMesh(const aiMesh* const p_mesh, const aiScene* const p_scene)
	{
		const std::string meshName = _name + "_" + std::string(p_mesh->mName.C_Str());
	
		if (VERBOSE)
			std::cout << "-- Loading mesh: " << meshName <<" number:"<< p_mesh->mNumVertices<< std::endl;

		// Load vertex attributes.
		std::vector<Vertex> vertices;
		vertices.resize(p_mesh->mNumVertices);

		for (unsigned int v = 0; v < p_mesh->mNumVertices; ++v)
		{
			Vertex& vertex = vertices[v];
			// Position.
			vertex._position.x = p_mesh->mVertices[v].x;
			vertex._position.y = p_mesh->mVertices[v].y;
			vertex._position.z = p_mesh->mVertices[v].z;
			// Normal.
			vertex._normal.x = p_mesh->mNormals[v].x;
			vertex._normal.y = p_mesh->mNormals[v].y;
			vertex._normal.z = p_mesh->mNormals[v].z;
			// Texture coordinates.
			if (p_mesh->HasTextureCoords(0))
			{
				vertex._texCoords.x = p_mesh->mTextureCoords[0][v].x;
				vertex._texCoords.y = p_mesh->mTextureCoords[0][v].y;
				// Tangent.
			//	vertex._tangent.x = p_mesh->mTangents[v].x;
			//	vertex._tangent.y = p_mesh->mTangents[v].y;
				//vertex._tangent.z = p_mesh->mTangents[v].z;
				// Bitangent.
				//vertex._bitangent.x = p_mesh->mBitangents[v].x;
				//vertex._bitangent.y = p_mesh->mBitangents[v].y;
			//	vertex._bitangent.z = p_mesh->mBitangents[v].z;
			}
			else
			{
				vertex._texCoords.x = 0.f;
				vertex._texCoords.y = 0.f;
			}
		}



		// Load indices.
		std::vector<unsigned int> indices;
		indices.resize(p_mesh->mNumFaces * 3); // Triangulated.
		for (unsigned int f = 0; f < p_mesh->mNumFaces; ++f)
		{
			const aiFace& face = p_mesh->mFaces[f];
			const unsigned int f3 = f * 3;
			indices[f3] = face.mIndices[0];
			indices[f3 + 1] = face.mIndices[1];
			indices[f3 + 2] = face.mIndices[2];
		}


		// Load materials.
		const aiMaterial* const mtl = p_scene->mMaterials[p_mesh->mMaterialIndex];
		aiString  texturePath;
		if (VERBOSE)
		{
			std::cout << "-- Mesh Loading Done! - Starting with Material --" << std::endl;
		}

		_nbTriangles += p_mesh->mNumFaces;
		_nbVertices += p_mesh->mNumVertices;


		//_meshes.emplace_back(std::move());
		std::string materialName = p_scene->mMaterials[p_mesh->mMaterialIndex]->GetName().C_Str();
		m_meshes.emplace(materialName, new TriangleMesh(meshName, vertices, indices, materialName,this));
		if (VERBOSE)
		{
			std::cout << "-- Done! "						  //
				<< indices.size() / 3 << " triangles, " //
				<< vertices.size() << " vertices." << std::endl;
		}
		
	
	}


	Material Model::_loadMaterial(const aiMaterial* const p_mtl)
	{
		aiString materialName;
			
		p_mtl->Get(AI_MATKEY_NAME, materialName);

		Material material{ materialName.C_Str() };

		aiColor3D color;
		aiString  texturePath;
		//LveTexture	  texture; // We suppose to have at most one texture per type.


		// ===================================================== AMBIENT
		if (p_mtl->GetTextureCount(aiTextureType_AMBIENT) > 0) // Texture ?
		{
			p_mtl->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);
			std::string completePath = _dirPath + texturePath.C_Str();

			if (VERBOSE)
				std::cout << "-*- Load Ambient Map:" << std::endl;

			material.setMaterialParameter(Material::EMaterialParameter::AMBIENTMAP, std::make_shared<LveTexture>(completePath));
		}
		else if (p_mtl->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) // else Material ?
		{
			//material._ambient = glm::vec3(color.r, color.g, color.b);
		}

		// ===================================================== NORMAL
		if (p_mtl->GetTextureCount(aiTextureType_NORMALS) > 0) // Texture ?
		{

			//p_mtl->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
			//std::string completePath = _dirPath + texturePath.C_Str();

			if(VERBOSE)
				std::cout << "-*- Load Normal Map:" << std::endl;

			//material.m_idNormal = addTexture(completePath);
			//material._hasNormalMap = true;

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

			if (VERBOSE)
				std::cout << "-*- Load Diffuse Map:" << std::endl;

			material.setMaterialParameter(Material::EMaterialParameter::DIFFUSEMAP, std::make_shared<LveTexture>(completePath));
		}
		else if (p_mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) // else Material ?
		{
			if (VERBOSE)
				std::cout << "DIFFUSE COLOR " << color.r<< color.b<< color.g<< std::endl;
			material.setMaterialParameter(Material::EMaterialParameter::DIFFUSECOLOR, glm::vec4(color.r, color.g, color.b,0.f));
		}




		// ===================================================== SPECULAR
		if (p_mtl->GetTextureCount(aiTextureType_SPECULAR) > 0) // Texture ?
		{
			p_mtl->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
			std::string completePath = _dirPath + texturePath.C_Str();

			if (VERBOSE)
				std::cout << "-*- Load Specular Map:" << std::endl;

			material.setMaterialParameter(Material::EMaterialParameter::SPECULARMAP, std::make_shared<LveTexture>(completePath));
		}
		else if (p_mtl->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) // else Material ?
		{
			material.setMaterialParameter(Material::EMaterialParameter::SPECULARCOLOR, glm::vec4(color.r, color.g, color.b, 0.f));
		}
		// =====================================================

		// ===================================================== SHININESS
		float shininess;
		if (p_mtl->GetTextureCount(aiTextureType_SHININESS) > 0) // Texture ?
		{
			p_mtl->GetTexture(aiTextureType_SHININESS, 0, &texturePath);

			//material._shininessMap = std::make_shared<LveTexture>( _dirPath + texturePath.C_Str());
			//Model::s_allTexturesName.emplace_back(_dirPath + texturePath.C_Str());
			//material._hasShininessMap = true;
		}
		else if (p_mtl->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) // else Material ?
		{
			material.setMaterialParameter(Material::EMaterialParameter::SHININESS, shininess);
		}

		float opacity;
		if (p_mtl->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
		{
			material.setMaterialParameter(Material::EMaterialParameter::OPACITY, opacity);
		}

		float indexOfRefraction;
		if (p_mtl->Get(AI_MATKEY_REFRACTI, indexOfRefraction) == AI_SUCCESS)
		{
			material.setMaterialParameter(Material::EMaterialParameter::REFRACTION, indexOfRefraction);
		}

		float transparency;
		if (p_mtl->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparency) == AI_SUCCESS)
		{
			material.setMaterialParameter(Material::EMaterialParameter::TRANSPARENCY, transparency);
		}

		float illuminationModel;
		if (p_mtl->Get(AI_MATKEY_SHADING_MODEL, illuminationModel) == AI_SUCCESS)
		{
			material.setMaterialParameter(Material::EMaterialParameter::ILLUMINATIONMODEL, illuminationModel);
		}

		material.setupDescriptorSet();
		return material;

	}
	int Model::addTexture(std::string completePath)
	{
		
		auto texturesMap = SceneManager::getInstance()->getTextureMap();
		auto textureId = SceneManager::getInstance()->getShaderTextureId();

		if (texturesMap.count(completePath) ==0) 
			return SceneManager::getInstance()->addTextureElement(completePath, new LveTexture(completePath));
		else {
			auto it = std::find(textureId.begin(), textureId.end(), completePath);

			if (it != textureId.end()) {
				return std::distance(textureId.begin(), it);
			}
		}
		return -1;
	}


	void Model::bind(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)
	{
		for (size_t i = 0; i < _meshes.size(); i++)
		{
		}
	}

	void Model::draw(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)
	{
		for (size_t i = 0; i < _meshes.size(); i++)
		{
			_meshes[i].bind(commandBuffer, p_frameIndex, p_pipelineLayout);

			_meshes[i].draw(commandBuffer);
		}
	}



}