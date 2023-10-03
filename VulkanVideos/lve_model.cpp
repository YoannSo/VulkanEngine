#include "lve_model.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>
#include "lve_utils.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std {
	template<>
	struct hash<lve::LveModel::Vertex> {
		size_t operator()(lve::LveModel::Vertex const& vertex)const {//overload du calcul de hash pour un type vertex
			size_t seed;
			lve::hashCombine(seed, vertex.position, vertex._color, vertex._normal, vertex.uv);
			return seed;
		}
	};
}

namespace lve {

	LveModel::LveModel(LveDevice& device, const LveModel::Builder& builder) : lveDevice{ device } {

		createVertexBuffers(builder._vertices);
		createIndexBuffer(builder._indices);
	}
	LveModel::~LveModel()
	{

	}
	std::unique_ptr<LveModel> LveModel::createModelFromFile(LveDevice& device, const std::string& filePath)
	{

		Builder builder{};
		builder.loadModel(filePath);
		std::cout << "Vertex count:" << builder._vertices.size() << "\n ";
		return std::make_unique<LveModel>(device, builder);

	}
	void LveModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { _vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);// record comand buffer, to bind one vertex buffer at index 0 with offset of 0
		

		if (_hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void LveModel::draw(VkCommandBuffer commandBuffer)
	{
		if (_hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0,0, 0);

		}else
		{
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

		}
	}
	void LveModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be a least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount; // total number of vertex buffer of all vertices
		uint32_t vertexSize = sizeof(vertices[0]);

		LveBuffer stagingBuffer{ lveDevice,vertexSize,vertexCount,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,//tell device that we want to create a buffer for vextes data
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |//allocate mem to be accessible from the host, host to write on device meem
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };//keep host and device memory consistent together, }

	

			stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());


		_vertexBuffer = std::make_unique<LveBuffer>(lveDevice, vertexSize, vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,//tell device that we want to create a buffer for vextes data
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		lveDevice.copyBuffer(stagingBuffer.getBuffer(), _vertexBuffer->getBuffer(), bufferSize);


	}
	void LveModel::createIndexBuffer(const std::vector<uint32_t>& indices)
	{
		

	}
	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions( 1 );//single vertex buffer
		bindingDescriptions[0].binding = 0;//index 0 
		bindingDescriptions[0].stride = sizeof(Vertex);//stride of vertex size by vertex
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}
	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescription()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

	/*	attributeDescriptions[0].binding = 0;//index 0 
		attributeDescriptions[0].location = 0;//location 0
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset =  offsetof(Vertex,position);;

		attributeDescriptions[1].binding = 0;//binding still 0 bc we are interleaved pos and color
		attributeDescriptions[1].location = 1;//location 1 bc match location use in the vertex
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex,_color);//bye offset of the color member*/
		return attributeDescriptions;
	}


	void LveModel::Builder::loadModel(const std::string& filePath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}
		std::cout << "SHAPE:" << shapes.size() << " "<< shapes.at(0).name<<std::endl;
		std::cout << "material:" << materials.size();
		
		_vertices.clear();
		_indices.clear();
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};
				if (index.vertex_index >= 0) {//check if there is there is something
					vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2] ,
					};
				}

					vertex._color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2] ,
					};
				

				if (index.normal_index >= 0) {//check if there is there is something
					vertex._normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2] ,
					};
				}

				if (index.texcoord_index >= 0) {//check if there is there is something
					vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}
				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex]= static_cast<uint32_t>(_vertices.size());
					_vertices.push_back(vertex);

				}
				_indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}