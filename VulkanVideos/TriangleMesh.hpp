#pragma once
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

	struct Material {
		glm::vec3 _ambient{ 0.f,0.f,0.f };
		glm::vec3 _diffuse{ 0.f,0.f,0.f };
		glm::vec3 _specular{ 0.f,0.f,0.f };
		float _shininess{ 0.f };

		bool _hasAmbientMap{ false };
		bool _hasDiffuseMap{ false };
		bool _hasSpecularMap{ false };
		bool _hasShininessMap{ false };

		std::shared_ptr<LveTexture> _ambientMap{ nullptr };//cab ne null ? 
		std::shared_ptr<LveTexture> _diffuseMap{ nullptr };
		std::shared_ptr<LveTexture> _specularMap{ nullptr };
		std::shared_ptr<LveTexture> _shininessMap{ nullptr };
	};

	class TriangleMesh {
	public:
		TriangleMesh() = delete;
		~TriangleMesh() = default;
		TriangleMesh(const TriangleMesh&) = default;
		//TriangleMesh(const TriangleMesh&) = delete; // delete copy constructor
		//TriangleMesh& operator=(const TriangleMesh&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed


		TriangleMesh( LveDevice& p_lveDevice,const std::string& p_name,
			const std::vector<Vertex>& p_vertices,
			const std::vector<unsigned int>& p_indices,
			const Material& p_material);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		void createVertexBuffers();
		void createIndexBuffer();
		void createImageDescriptor();


	private:

		LveDevice& _lveDevice;

		std::string _name{ "Unknow" };
		std::vector<Vertex> _vertices;

		std::vector<glm::vec3> _positions;

		std::vector<uint32_t> _indices;


		Material _material;

		uint32_t _indexCount{ 0 };
		uint32_t _vertexCount{ 0 };

		VkDescriptorImageInfo _specularMapDescriptorInfo{};
		VkDescriptorImageInfo _ambientMapDescriptorInfo{};
		VkDescriptorImageInfo _diffuseMapDescriptorInfo{};
		VkDescriptorImageInfo _shininessMapDescriptorInfo{};

		std::shared_ptr<LveBuffer> _vertexBuffer{ nullptr };

		//LveDescriptorSetLayout* _descriptorSet{};//bug

		bool _hasIndexBuffer{ false };
		std::shared_ptr<LveBuffer> _indexBuffer{ nullptr };

	};
}