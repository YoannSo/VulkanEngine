#pragma once
#ifndef MODEL_HPP
#define MODEL_HPP
#include "TriangleMesh.hpp"
#include "lve_device.hpp"
#include <vector>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postProcess.h>
#include <assimp/scene.h>

#include "lve_swap_chain.hpp"
#include "define.hpp"
#include <glm.hpp>
#include "Material.h"
#include <map>

#include "GameObject.hpp"
#include "MaterialManager.h"

namespace lve {
	class Model: public GameObject{



	public:
		Model(const std::string p_name, const aiScene& p_scene,const MaterialManager& p_materialManagerRef);
		~Model();


		void bind(VkCommandBuffer& commandBuffer, int& p_frameIndex,VkPipelineLayout& p_pipelineLayout)override;
		void draw(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)override;

		static std::vector<std::string> LoadMaterials(const aiScene& p_scene);

		const std::vector<TriangleMesh>& getMeshes() const { return m_meshes; }

	private:

		void	 _loadMesh(const aiMesh* const p_mesh, const aiScene& p_scene,const MaterialManager& p_materialManagerRef);
		
		void setupDescriptorPool();

		void setupTexDescriptorPool();



	private:

		std::string _name{"UNDEFINED"};
		
		std::vector<TriangleMesh> m_meshes;


		uint32_t _nbTriangles{ 0 };
		uint32_t _nbVertices{ 0 };
		uint32_t _nbMeshes{ 0 };
		uint32_t m_nbMaterials{ 0 };

	};
}
#endif