#pragma once
#ifndef TRIANGLE_MESH_HPP
#define TRIANGLE_MESH_HPP
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "lve_buffer.hpp"
#include <string>
#include "lve_texture.hpp"
#include "lve_descriptor.hpp"

namespace lve {

	struct Vertex {
		glm::vec3 _position;
		glm::vec3 _normal;
		glm::vec3 _color{ 1.f,1.f,1.f };
		//glm::vec3 _tangent;
		//glm::vec3 _bitangent;
		glm::vec2 _texCoords;

		bool operator==(const Vertex& other)const {
			return _position == other._position && _normal == other._normal && _texCoords == other._texCoords;
		}
		static std::vector<VkVertexInputBindingDescription>getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription>getAttributeDescription();
	};


	struct ObjectUbo {
		int diffuseTextureID=-1;
	};


	class TriangleMesh {
	public:
		TriangleMesh() = delete;
		~TriangleMesh();
		//TriangleMesh(const TriangleMesh&) = default;
		//TriangleMesh(const TriangleMesh&) = delete; // delete copy constructor
		//TriangleMesh& operator=(const TriangleMesh&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed


		 // Constructeur de copie désactivé
		TriangleMesh(const TriangleMesh& other) = delete;

		// Opérateur d'assignation de copie désactivé
		TriangleMesh& operator=(const TriangleMesh& other) = delete;

		// Constructeur de déplacement
		TriangleMesh(TriangleMesh&& other) noexcept = default;

		// Opérateur d'assignation de déplacement
		TriangleMesh& operator=(TriangleMesh&& other) noexcept = default;


		TriangleMesh(const std::string& p_name,
			const std::vector<Vertex>& p_vertices,
			const std::vector<unsigned int>& p_indices,
			const std::string p_material);

		void bind(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout);
		void draw(VkCommandBuffer& commandBuffer);

		void createVertexBuffers();
		void createIndexBuffer();
		void setupObjectDescriptor();

		inline std::vector<VkDescriptorSet> getDescriptorSet() { return m_objectDescriptorSet; }




	private:

		ObjectUbo createObjectUbo();
		

	private:

		std::string _name{ "Unknow" };
		std::vector<Vertex> _vertices;

		std::vector<glm::vec3> _positions;

		std::vector<uint32_t> _indices;
		std::string m_materialName;


		uint32_t _indexCount{ 0 };
		uint32_t _vertexCount{ 0 };

		std::unique_ptr<VkDescriptorImageInfo> _specularMapDescriptorInfo{nullptr};
		std::unique_ptr<VkDescriptorImageInfo> _ambientMapDescriptorInfo{ nullptr };
		std::unique_ptr<VkDescriptorImageInfo> _diffuseMapDescriptorInfo{ nullptr };
		std::unique_ptr<VkDescriptorImageInfo> _shininessMapDescriptorInfo{ nullptr };


	
		std::vector<LveBuffer*> m_objectUbo;
		std::vector<VkDescriptorSet> m_objectDescriptorSet{};



		std::shared_ptr<LveBuffer> _vertexBuffer{ nullptr };

		bool _hasIndexBuffer{ false };
		std::shared_ptr<LveBuffer> _indexBuffer{ nullptr };
		

	};
}
#endif
