#include "LightSystem.h"

void lve::LightSystem::createDescriptorLayout()
{
	m_descriptorLayout = LveDescriptorSetLayout::Builder()
		.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
}

lve::LightSystem::LightSystem(const LightManager& p_lightManager, LveDescriptorPool& p_descriptorPool):
	m_lightManager(p_lightManager),
	m_descriptorPool(p_descriptorPool)
{
	createDescriptorLayout();
}

lve::LightSystem::~LightSystem()
{

}

void lve::LightSystem::createDescriptors()
{

	std::vector<LightGPU> lightsBuffer = m_lightManager.getLightsBufferData();


	m_lightBuffer = std::make_unique<LveBuffer>(
		sizeof(LightGPU)* lightsBuffer.size(),                            // instanceSize
		1,                     // instanceCount
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);


	std::cout << "Number of lights in scene: " << lightsBuffer.data() <<" size " << lightsBuffer[0].position.w << std::endl;
	m_lightBuffer->map();
	m_lightBuffer->writeToBuffer(lightsBuffer.data(), sizeof(LightGPU) * lightsBuffer.size());
	m_lightBuffer->flush();

	m_descriptorSet.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);


	for (auto& descriptorSet : m_descriptorSet) {
		auto bufferUboInfo = m_lightBuffer->descriptorInfo();
		LveDescriptorWriter(*m_descriptorLayout, m_descriptorPool)
			.writeBuffer(0, &bufferUboInfo)
			.build(descriptorSet);
	}
}
