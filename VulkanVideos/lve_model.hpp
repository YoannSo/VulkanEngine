#pragma once
#include "lve_device.hpp"
#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1
#include <glm/glm.hpp>
#include "lve_buffer.hpp"
#include <memory>
#include <vector>
namespace lve {
	class LveModel {//take  vertex data, and allocate memory and copy on the device to be render
	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 _color;
			glm::vec3 _normal{};
			glm::vec2 uv{};
			static std::vector<VkVertexInputBindingDescription>getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription>getAttributeDescription();

			bool operator==(const Vertex& other)const {
				return position == other.position && _color == other._color && _normal == other._normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> _vertices{};
			std::vector<uint32_t> _indices{};

			void loadModel(const std::string& filePath);
		};


  LveModel( const LveModel::Builder &builder);
		~LveModel();
		  LveModel(const LveModel &) = delete;
  LveModel& operator=(const LveModel &) = delete;


  static std::unique_ptr<LveModel> createModelFromFile( const std::string& filePath);

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);


  static std::vector<std::string> m_allTexturesNames;
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>&indices);
		
				//vkbuffer and vkdevicememory are 2 different object
		 std::unique_ptr<LveBuffer> _vertexBuffer;
		uint32_t vertexCount;
		bool _hasIndexBuffer = false;
		std::unique_ptr<LveBuffer> _indexBuffer;
		uint32_t _indexCount;

	};
}