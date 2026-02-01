#include <Engine/Systems/LightSystem.h>

void engine::LightSystem::createDescriptorLayout()
{
	m_descriptorLayout = DescriptorSetLayout::Builder()
		.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
}

engine::LightSystem::LightSystem(const LightManager& p_lightManager, DescriptorPool& p_descriptorPool):
	m_lightManager(p_lightManager),
	m_descriptorPool(p_descriptorPool)
{
	createDescriptorLayout();
}

engine::LightSystem::~LightSystem()
{

}
void engine::LightSystem::setupDescriptorSet()
{

	std::vector<LightGPU> lightsBuffer = m_lightManager.getLightsBufferData();


	m_lightBuffer = std::make_unique<GwatBuffer>(
		sizeof(LightGPU)* lightsBuffer.size(),                            // instanceSize
		1,                     // instanceCount
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);


	std::cout << "Number of lights in scene: " << lightsBuffer.data() <<" size " << lightsBuffer[0].position.w << std::endl;
	m_lightBuffer->map();
	m_lightBuffer->writeToBuffer(lightsBuffer.data(), sizeof(LightGPU) * lightsBuffer.size());
	m_lightBuffer->flush();

	m_descriptorSet.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);


	for (auto& descriptorSet : m_descriptorSet) {
		auto bufferUboInfo = m_lightBuffer->descriptorInfo();
		DescriptorWriter(*m_descriptorLayout, m_descriptorPool)
			.writeBuffer(0, &bufferUboInfo)
			.build(descriptorSet);
	}
}
