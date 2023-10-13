#include "TriangleMesh.hpp"
#include "lve_swap_chain.hpp"
lve::TriangleMesh::~TriangleMesh()
{
	
}

lve::TriangleMesh::TriangleMesh( LveDevice &p_lveDevice , const std::string& p_name, const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, const Material& p_material)
	:_lveDevice{ p_lveDevice }, _name {p_name}, _vertices{ p_vertices }, _material{ p_material }, _indices{ p_indices }
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
	createImageDescriptor();
    //_descriptorSet = LveDescriptorSetLayout::Builder(_lveDevice).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build().get();

}

void lve::TriangleMesh::bind(VkCommandBuffer& commandBuffer, int& p_frameIndex, VkPipelineLayout& p_pipelineLayout)
{
	if(this->_material._hasDiffuseMap)
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_pipelineLayout,
		1,//strating sert
		1,//number of set
		&_globalDescriptorSets[p_frameIndex],
		0,
		nullptr
	);

	VkBuffer buffers[] = { _vertexBuffer->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);// record comand buffer, to bind one vertex buffer at index 0 with offset of 0


	if (_hasIndexBuffer) {
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

	}
}

void lve::TriangleMesh::draw(VkCommandBuffer& commandBuffer)
{

	if (_hasIndexBuffer) {
		vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);

	}
	else
	{
		vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);

	}
}

void lve::TriangleMesh::createVertexBuffers()
{
	_vertexCount = static_cast<uint32_t>(_vertices.size());
	assert(_vertexCount >= 3 && "Vertex count must be a least 3");
	VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertexCount; // total number of vertex buffer of all vertices
	uint32_t vertexSize = sizeof(_vertices[0]);

	LveBuffer stagingBuffer{ _lveDevice,vertexSize,_vertexCount,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,//tell device that we want to create a buffer for vextes data
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |//allocate mem to be accessible from the host, host to write on device meem
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };//keep host and device memory consistent together, }



	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)_vertices.data());


	_vertexBuffer = std::make_unique<LveBuffer>(_lveDevice, vertexSize, _vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,//tell device that we want to create a buffer for vextes data
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	_lveDevice.copyBuffer(stagingBuffer.getBuffer(), _vertexBuffer->getBuffer(), bufferSize);



}

void lve::TriangleMesh::createIndexBuffer()
{
	_indexCount= static_cast<uint32_t>(_indices.size());
	_hasIndexBuffer = _indexCount > 0;

	if (!_hasIndexBuffer) {
		return;
	}

	VkDeviceSize bufferSize = sizeof(_indices[0]) * _indexCount;
	uint32_t indexSize = sizeof(_indices[0]);

	LveBuffer stagingBuffer{
		_lveDevice,
		indexSize,
		_indexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)_indices.data());

	_indexBuffer = std::make_unique<LveBuffer>(
		_lveDevice,
		indexSize,
		_indexCount,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	_lveDevice.copyBuffer(stagingBuffer.getBuffer(), _indexBuffer->getBuffer(), bufferSize);

}
void lve::TriangleMesh::createImageDescriptor()
{
	if (this->_material._hasAmbientMap) {

		_ambientMapDescriptorInfo =std::make_unique< VkDescriptorImageInfo>();
		_ambientMapDescriptorInfo->sampler = this->_material._ambientMap->getSampler();
		_ambientMapDescriptorInfo->imageView = this->_material._ambientMap->getImageView();
		_ambientMapDescriptorInfo->imageLayout = this->_material._ambientMap->getImageLayout();
	}
	if (this->_material._hasDiffuseMap) {
		std::cout << "test" << std::endl;
		_diffuseMapDescriptorInfo = std::make_unique< VkDescriptorImageInfo>();
		_diffuseMapDescriptorInfo->sampler = this->_material._diffuseMap->getSampler();
		_diffuseMapDescriptorInfo->imageView = this->_material._diffuseMap->getImageView();
		_diffuseMapDescriptorInfo->imageLayout = this->_material._diffuseMap->getImageLayout();
	}
	if (this->_material._hasSpecularMap) {
		_specularMapDescriptorInfo = std::make_unique< VkDescriptorImageInfo>();
		_specularMapDescriptorInfo->sampler = this->_material._specularMap->getSampler();
		_specularMapDescriptorInfo->imageView = this->_material._specularMap->getImageView();
		_specularMapDescriptorInfo->imageLayout = this->_material._specularMap->getImageLayout();
	}
	if (this->_material._hasShininessMap) {
		_shininessMapDescriptorInfo = std::make_unique< VkDescriptorImageInfo>();
		_shininessMapDescriptorInfo->sampler = this->_material._shininessMap->getSampler();
		_shininessMapDescriptorInfo->imageView = this->_material._shininessMap->getImageView();
		_shininessMapDescriptorInfo->imageLayout = this->_material._shininessMap->getImageLayout();
	}
}
void lve::TriangleMesh::setupDescriptorSetLayout(std::unique_ptr<LveDescriptorPool>& p_descriptorPool, std::unique_ptr<LveDescriptorSetLayout>& p_descriptorSetLayout)
{
	if (this->_material._hasDiffuseMap) {
		_globalDescriptorSets.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::cout << this->_name << std::endl;
		for (int i = 0; i < _globalDescriptorSets.size(); i++) {
			LveDescriptorWriter(*p_descriptorSetLayout, *p_descriptorPool).
				writeImage(0, _diffuseMapDescriptorInfo.get())
				.build(_globalDescriptorSets[i]);
		}
	}
//
}
std::vector<VkVertexInputBindingDescription> lve::Vertex::getBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);//single vertex buffer
	bindingDescriptions[0].binding = 0;//index 0 
	bindingDescriptions[0].stride = sizeof(Vertex);//stride of vertex size by vertex
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}
std::vector<VkVertexInputAttributeDescription> lve::Vertex::getAttributeDescription()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _position) });
	attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _color) });
	attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _normal) });
	attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, _texCoords) });

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