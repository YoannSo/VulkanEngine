#include <Engine/Objects/Model.hpp>
namespace engine {


	Model::Model(const std::string p_name, const aiScene& p_scene,const MaterialManager& p_materialManagerRef, bool p_useBasicMaterial) :GameObject(), _name{ p_name }
	{

		_nbMeshes = p_scene.mNumMeshes;
		std::cout << "--** Your model got " << _nbMeshes << " meshes **--" << std::endl;

		for (uint32_t i = 0; i < _nbMeshes; ++i) {
			_loadMesh(p_scene.mMeshes[i], p_scene, p_materialManagerRef, p_useBasicMaterial);
		}


	}
	Model::~Model()
	{
	}

	void Model::_loadMesh(const aiMesh* const p_mesh, const aiScene&  p_scene, const MaterialManager& p_materialManagerRef, bool p_useBasicMaterial)
	{
		const std::string meshName = _name + "_" + std::string(p_mesh->mName.C_Str());

		if (VERBOSE)
			std::cout << "-- Loading mesh: " << meshName << " number:" << p_mesh->mNumVertices << std::endl;

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
				vertex._tangent.x = p_mesh->mTangents[v].x;
				vertex._tangent.y = p_mesh->mTangents[v].y;
				vertex._tangent.z = p_mesh->mTangents[v].z;
				// Bitangent.
				vertex._bitangent.x = p_mesh->mBitangents[v].x;
				vertex._bitangent.y = p_mesh->mBitangents[v].y;
				vertex._bitangent.z = p_mesh->mBitangents[v].z;

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
		const aiMaterial* const mtl = p_scene.mMaterials[p_mesh->mMaterialIndex];
		aiString  texturePath;
		if (VERBOSE)
		{
			std::cout << "-- Mesh Loading Done! - Starting with Material --" << std::endl;
		}

		_nbTriangles += p_mesh->mNumFaces;
		_nbVertices += p_mesh->mNumVertices;


		//_meshes.emplace_back(std::move());
		size_t textureID = 0;
		if (!p_useBasicMaterial) {
			std::string materialName = p_scene.mMaterials[p_mesh->mMaterialIndex]->GetName().C_Str();
			textureID = p_materialManagerRef.getIDOfMaterial(materialName);
		}


		m_meshes.emplace_back(TriangleMesh(meshName, vertices, indices, textureID, this));//la pas mettre 0

		if (VERBOSE)
		{
			std::cout << "-- Done! "						  //
				<< indices.size() / 3 << " triangles, " //
				<< vertices.size() << " vertices." << std::endl;
		}


	}

	void Model::bind(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i].bind(commandBuffer, p_frameIndex, p_pipelineLayout);
		}
	}

	void Model::draw(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)
	{

		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i].draw(commandBuffer);
		}
	}



}