#include "BillboardSystem.h"

namespace engine::systems
{

	BillboardSystem::BillboardSystem(const BillboardManager& p_billboardManager, DescriptorPool& p_descriptorPool) :
		m_billboardManager(p_billboardManager),
		m_descriptorPool(p_descriptorPool)
	{
		createDescriptorLayout();
	}


	void BillboardSystem::createDescriptors()
	{

		std::vector<BillboardManager::BillboardCPU> billboardsData = m_billboardManager.getBillboardsBufferData();


		m_billboardBuffer = std::make_unique<GwatBuffer>(
			sizeof(BillboardManager::BillboardCPU) * billboardsData.size(),                            // instanceSize
			1,                     // instanceCount
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);


		m_billboardBuffer->map();
		m_billboardBuffer->writeToBuffer(billboardsData.data(), sizeof(BillboardManager::BillboardCPU) * billboardsData.size());
		m_billboardBuffer->flush();

		m_descriptorSet.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);


		for (auto& descriptorSet : m_descriptorSet) {
			auto bufferUboInfo = m_billboardBuffer->descriptorInfo();
			DescriptorWriter(*m_descriptorLayout, m_descriptorPool)
				.writeBuffer(0, &bufferUboInfo)
				.build(descriptorSet);
		}
	}
	void BillboardSystem::createDescriptorLayout()
	{
		m_descriptorLayout = DescriptorSetLayout::Builder()
			.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();
	}



}