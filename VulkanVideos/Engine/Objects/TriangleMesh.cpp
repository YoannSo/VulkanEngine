#include "TriangleMesh.hpp"

engine::TriangleMesh::~TriangleMesh()
{
	
}


engine::TriangleMesh::TriangleMesh(const std::string& p_name, const std::vector<Vertex>& p_vertices, const std::vector<unsigned int>& p_indices, const uint32_t p_materialID, const Model* p_parent)
	: _name{ p_name }, _vertices{ p_vertices }, _materialID{ p_materialID }, _indices{ p_indices },m_modelRef{ p_parent }
{
	_indexCount = _indices.size();
	_vertexCount = _vertices.size();
	_positions.reserve(_vertexCount);
	for (Vertex v : _vertices) {
		_positions.emplace_back(v._position);
	}
	_positions.shrink_to_fit();
	createVertexBuffers();
	createIndexBuffer();
}


void engine::TriangleMesh::bind(const VkCommandBuffer& commandBuffer, const int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)const
{
	


	VkBuffer buffers[] = { _vertexBuffer->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);// record comand buffer, to bind one vertex buffer at index 0 with offset of 0


	

	if (_hasIndexBuffer) {
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

	}
}

void engine::TriangleMesh::draw(const VkCommandBuffer& commandBuffer)const
{

	if (_hasIndexBuffer) {
		vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);

	}
	else
	{
		vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);

	}
}

void engine::TriangleMesh::createVertexBuffers()
{
	_vertexCount = static_cast<uint32_t>(_vertices.size());
	assert(_vertexCount >= 3 && "Vertex count must be a least 3");
	VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertexCount; // total number of vertex buffer of all vertices
	uint32_t vertexSize = sizeof(_vertices[0]);

	GwatBuffer stagingBuffer{vertexSize,_vertexCount,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,//tell device that we want to create a buffer for vextes data
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |//allocate mem to be accessible from the host, host to write on device meem
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };//keep host and device memory consistent together, }



	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)_vertices.data());


	_vertexBuffer = std::make_unique<GwatBuffer>( vertexSize, _vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,//tell device that we want to create a buffer for vextes data
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Device::getInstance()->copyBuffer(stagingBuffer.getBuffer(), _vertexBuffer->getBuffer(), bufferSize);



}

void engine::TriangleMesh::createIndexBuffer()
{
	_indexCount= static_cast<uint32_t>(_indices.size());
	_hasIndexBuffer = _indexCount > 0;

	if (!_hasIndexBuffer) {
		return;
	}

	VkDeviceSize bufferSize = sizeof(_indices[0]) * _indexCount;
	uint32_t indexSize = sizeof(_indices[0]);

	GwatBuffer stagingBuffer{
		indexSize,
		_indexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)_indices.data());

	_indexBuffer = std::make_unique<GwatBuffer>(
		indexSize,
		_indexCount,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Device::getInstance()->copyBuffer(stagingBuffer.getBuffer(), _indexBuffer->getBuffer(), bufferSize);

}

std::vector<VkVertexInputBindingDescription> engine::Vertex::getBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);//single vertex buffer
	bindingDescriptions[0].binding = 0;//index 0 
	bindingDescriptions[0].stride = sizeof(Vertex);//stride of vertex size by vertex
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}
std::vector<VkVertexInputAttributeDescription> engine::Vertex::getAttributeDescription()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _position) });
	attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _color) });
	attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _normal) });
	attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, _texCoords) });
	attributeDescriptions.push_back({ 4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _tangent) });
	attributeDescriptions.push_back({ 5, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _bitangent) });

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