#pragma once
#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postProcess.h>
#include <assimp/scene.h>
#include <glm.hpp>

#include "define.hpp"
#include "GameObject.hpp"
#include "MaterialManager.h"
#include "TriangleMesh.hpp"
namespace engine {
	class Model: public GameObject{



	public:
		Model(const std::string p_name, const aiScene& p_scene,const MaterialManager& p_materialManagerRef, bool p_useBasicMaterial=false);
		~Model();

		void bind(VkCommandBuffer& commandBuffer, int& p_frameIndex,VkPipelineLayout& p_pipelineLayout)override;
		void draw(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)override;

		const std::vector<TriangleMesh>& getMeshes() const { return m_meshes; }

	private:
		void _loadMesh(const aiMesh* const p_mesh, const aiScene& p_scene,const MaterialManager& p_materialManagerRef, bool p_useBasicMaterial=false);

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