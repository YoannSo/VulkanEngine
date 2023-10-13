#include "model.hpp"
#include "lve_swap_chain.hpp"
#include <iostream>
#include "define.hpp"
#include <glm/glm.hpp>

namespace lve {
	
	//std::unique_ptr<LveDescriptorPool> Model::_modelDescriptorPool = nullptr;


	Model::Model(LveDevice& device, const std::string p_name, const std::string p_filePath) :_lveDevice{ device }, _name { p_name }, _filePath{ p_filePath }
	{
		std::cout << "Loading model " << _name << " from: " << p_filePath << std::endl;



		Assimp::Importer importer;
		_dirPath = "models/" + _name + "/";
		const unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs;

		const aiScene* const scene = importer.ReadFile(p_filePath, flags);

		if (scene == nullptr)
		{
			throw std::runtime_error("Fail to load file \" " + p_filePath + "\": " + importer.GetErrorString());
		}
		_nbMeshes = scene->mNumMeshes;
		std::cout <<  "--** Your model got " <<  _nbMeshes << " meshes **--" << std::endl;


		for (uint32_t i = 0; i < _nbMeshes; ++i) {
			_loadMesh(scene->mMeshes[i], scene);
		}

		setupDescriptorPool();

		for (auto& mesh: _meshes) {
			mesh.setupDescriptorSetLayout(_modelDescriptorPool, _modelDescriptorLayout);
		}

	}
	Model::~Model()
	{
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
		Material				 material;
		if (mtl == nullptr)
		{
			std::cerr << "[WARNING] - Loading mesh: " << meshName << std::endl;
			std::cerr << "=> Material undefined," << meshName << " assigned to default material" << std::endl;
		}
		else
		{
			material = _loadMaterial(mtl);
		}

		_nbTriangles += p_mesh->mNumFaces;
		_nbVertices += p_mesh->mNumVertices;

		_meshes.emplace_back(std::move(TriangleMesh(_lveDevice,meshName, vertices, indices, material)));

		if (VERBOSE)
		{
			std::cout << "-- Done! "						  //
				<< indices.size() / 3 << " triangles, " //
				<< vertices.size() << " vertices." << std::endl;
		}
		
	
	}

	Material Model::_loadMaterial(const aiMaterial* const p_mtl)
	{
		Material material;

		aiColor3D color;
		aiString  texturePath;
		//LveTexture	  texture; // We suppose to have at most one texture per type.


		// ===================================================== AMBIENT
		if (p_mtl->GetTextureCount(aiTextureType_AMBIENT) > 0) // Texture ?
		{
			p_mtl->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);
			std::cout << texturePath.C_Str() << std::endl;

			material._ambientMap = std::make_shared<LveTexture>(_lveDevice, _dirPath + texturePath.C_Str());
			material._hasAmbientMap = true;
			
		}
		else if (p_mtl->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) // else Material ?
		{
			material._ambient = glm::vec3(color.r, color.g, color.b);
		}



		// ===================================================== DIFFUSE
		if (p_mtl->GetTextureCount(aiTextureType_DIFFUSE) > 0) // Texture ?
		{
			p_mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			std::cout << _dirPath + texturePath.C_Str() << std::endl;

				material._diffuseMap = std::make_shared<LveTexture>(_lveDevice, _dirPath + texturePath.C_Str());
				material._hasDiffuseMap = true;

		}
		else if (p_mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) // else Material ?
		{
			material._diffuse = glm::vec3(color.r, color.g, color.b);


		}




		// ===================================================== SPECULAR
		if (p_mtl->GetTextureCount(aiTextureType_SPECULAR) > 0) // Texture ?
		{
			p_mtl->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
			material._specularMap = std::make_shared<LveTexture>(_lveDevice, _dirPath + texturePath.C_Str());
			material._hasSpecularMap = true;
		}
		else if (p_mtl->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) // else Material ?
		{
			material._specular = glm::vec3(color.r, color.g, color.b);
		}
		// =====================================================

		// ===================================================== SHININESS
		float shininess;
		if (p_mtl->GetTextureCount(aiTextureType_SHININESS) > 0) // Texture ?
		{
			p_mtl->GetTexture(aiTextureType_SHININESS, 0, &texturePath);

			material._shininessMap = std::make_shared<LveTexture>(_lveDevice, _dirPath + texturePath.C_Str());
			material._hasShininessMap = true;
		}
		else if (p_mtl->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) // else Material ?
		{
			material._shininess = shininess;
		}

		return material;

	}
	void Model::setupDescriptorPool()
	{
		uint32_t numberOfTextures = 0;
		for (auto& mesh : this->_meshes) {
			if (mesh._material._hasAmbientMap)
				numberOfTextures++;
			if (mesh._material._hasDiffuseMap)
				numberOfTextures++;
			if (mesh._material._hasShininessMap)
				numberOfTextures++;
			if (mesh._material._hasSpecularMap)
				numberOfTextures++;
		}

		auto builder=LveDescriptorPool::Builder(_lveDevice);
		builder.setMaxSets(this->_meshes.size()*2);
		builder.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, numberOfTextures* LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		builder.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, this->_meshes.size() * LveSwapChain::MAX_FRAMES_IN_FLIGHT);



		_modelDescriptorPool = builder.build();


		_modelDescriptorLayout = LveDescriptorSetLayout::Builder(_lveDevice).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();


	}
	/*LveTexture Model::_loadTexture(const aiString& p_path, const std::string& p_type) {

		const char* entryPath = p_path.C_Str();

		if (VERBOSE)
			std::cout << "Loading texture (" << p_type << "): " << entryPath << std::endl;
	
		for (size_t i = 0; i < _textures.size(); ++i)
		{
			if (std::strcmp(_textures[i].getPath().data(), entryPath) == 0)
			{
				if (VERBOSE)
				{
					std::cout << "-> Already loaded !" << std::endl;
				}
				if (_textures[i].getType() == p_type)
				{
					return _textures[i];
				}
				else // One texture can be used for more than one type.
				{
				
					return LveTexture(_textures[i],p_type);

				}
			}
		}
	}*/



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