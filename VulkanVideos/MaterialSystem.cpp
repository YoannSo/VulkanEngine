#include "MaterialSystem.h"

lve::MaterialSystem::MaterialSystem(const TextureManager& p_textureManager, const MaterialManager& p_materialManager, LveDescriptorPool& p_descriptorPool) :
	m_textureManager{ p_textureManager },
	m_materialManager{ p_materialManager },
	m_device{ LveDevice::getInstance() },
	m_descriptorPool{ p_descriptorPool }
{
}

lve::MaterialSystem::~MaterialSystem()
{
}

void lve::MaterialSystem::setupDescriptorSetLayout()
{
	createTextureDescriptorLayout();
	createMaterialDescriptorLayout();
}
void lve::MaterialSystem::setupDescriptorSet()
{
	createMaterialDescriptors();
	createTextureDescriptors();
}

void lve::MaterialSystem::createTextureDescriptors()
{

	size_t numTextures = 0;

	std::vector<VkDescriptorImageInfo> imageInfos;
	m_textureManager.fillDescriptorImageInfoArray(imageInfos);

	m_bindlessTextureSet.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for(auto& descriptorSet : m_bindlessTextureSet) {
		if (imageInfos.size() > 0)
			LveDescriptorWriter(*m_bindlessTextureLayout, m_descriptorPool)
			.writeImages(0, imageInfos)
			.build(descriptorSet);
		else
			LveDescriptorWriter(*m_bindlessTextureLayout, m_descriptorPool)
			.build(descriptorSet);
	}


}

void lve::MaterialSystem::createMaterialDescriptors()
{

	std::vector<Material::MaterialGPU> materialBufferData;
	m_materialManager.fillMaterialsBufferData(materialBufferData);


	m_materialBuffer = std::make_unique<LveBuffer>(
		sizeof(Material::MaterialGPU),                            // instanceSize
		m_materialManager.getMaterialCount(),                     // instanceCount
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);	
	
	m_materialBuffer->map();
	m_materialBuffer->writeToBuffer(materialBufferData.data(), VK_WHOLE_SIZE);
	m_materialBuffer->flush();

	m_materialDescriptorSets.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);


	for (auto& descriptorSet : m_materialDescriptorSets) {
		auto bufferUboInfo = m_materialBuffer->descriptorInfo();
		LveDescriptorWriter(*m_materialLayout, m_descriptorPool)
			.writeBuffer(0, &bufferUboInfo)
			.build(descriptorSet);
	}

		


	
}


void lve::MaterialSystem::createTextureDescriptorLayout()
{
	m_bindlessTextureLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, m_textureManager.getTextureCount()).build();

}

void lve::MaterialSystem::createMaterialDescriptorLayout()
{
	m_materialLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 1).build();

}
