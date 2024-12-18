#pragma once

#include "TriangleMesh.hpp"
#include "lve_device.hpp"
#include <vector>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postProcess.h>
#include <assimp/scene.h>
#include "lve_swap_chain.hpp"
#include "define.hpp"
#include <glm/glm.hpp>
#include "Material.h"
#include <map>

#include "GameObject.hpp"

namespace lve {
	class Model: public GameObject{



	public:
		Model(const std::string p_name,const std::string p_filePath);
		~Model();


		void bind(VkCommandBuffer& commandBuffer, int& p_frameIndex,VkPipelineLayout& p_pipelineLayout);
		void draw(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout);




		static std::vector<std::string> LoadMaterials(const aiScene* p_scene);
		std::vector<TriangleMesh*> getAllMeshesFromMaterial(const std::string p_materialName);


	private:

		void	 _loadMesh(const aiMesh* const p_mesh, const aiScene* const p_scene);
		Material _loadMaterial(const aiMaterial* const p_mtl);
		
		void setupDescriptorPool();

		int addTexture(std::string completePath);
		void setupTexDescriptorPool();



	private:



		std::string _name{"UNDEFINED"};
		
		std::vector<TriangleMesh> _meshes;

		std::multimap<std::string, TriangleMesh*> m_meshes;

		uint32_t _nbTriangles{ 0 };
		uint32_t _nbVertices{ 0 };
		uint32_t _nbMeshes{ 0 };
		uint32_t m_nbMaterials{ 0 };

		std::string _filePath;
		std::string _dirPath;

	};
}
