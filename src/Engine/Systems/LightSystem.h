#pragma once

#include <glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

#include <Vulkan/Buffer.hpp>
#include<Vulkan/Descriptor.hpp>
#include <Vulkan/SwapChain.hpp>
#include <Engine/Managers/LightManager.h>
#include "IGpuSyncable.h"
namespace engine
{
    class LightSystem : public IGpuSyncable
	{
		struct LightBufferGPU {
			uint32_t lightCount;
			uint32_t _pad[3]; // force offset = 16
			LightGPU lights[LightManager::MAX_LIGHT_IN_SCENE];
		};

	public:
		LightSystem(LightManager& p_lightManager,DescriptorPool& p_descriptorPool);
		~LightSystem();

		void syncBuffer() override;

		void fillDescriptorSetLayouts(std::vector<VkDescriptorSetLayout>& p_layouts) const
		{
			p_layouts.push_back(m_descriptorLayout->getDescriptorSetLayout());
		}

		const std::vector<VkDescriptorSet>& getDescriptorSet() const {
			return m_descriptorSet;

		}
		VkDescriptorSetLayout getDescriptorSetLayout() const {
			return m_descriptorLayout->getDescriptorSetLayout();
		}
		void setupDescriptorSet();

	private:
		void createDescriptorLayout();



	private:
		std::unique_ptr<DescriptorSetLayout> m_descriptorLayout;
		std::vector<VkDescriptorSet> m_descriptorSet;
       

		// Buffers GPU
		std::unique_ptr<GwatBuffer> m_lightBuffer;
		LightManager& m_lightManager;

		DescriptorPool& m_descriptorPool;
		const Device* m_device;

	};
}