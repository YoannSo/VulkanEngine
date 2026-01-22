#pragma once

#include <glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

#include "lve_buffer.hpp"
#include "lve_descriptor.hpp"
#include "lve_swap_chain.hpp"
#include "LightManager.h"
namespace lve
{
	class LightSystem
	{

		struct LightBufferGPU {
			uint32_t lightCount;
			uint32_t _pad[3]; // force offset = 16
			LightGPU lights[LightManager::MAX_LIGHT_IN_SCENE];
		};

	public:
		LightSystem(const LightManager& p_lightManager,LveDescriptorPool& p_descriptorPool);
		~LightSystem();


		void fillDescriptorSetLayouts(std::vector<VkDescriptorSetLayout>& p_layouts) const
		{
			p_layouts.push_back(m_descriptorLayout->getDescriptorSetLayout());
		}

		const std::vector<VkDescriptorSet>& getDescriptorSet() const {

			return m_descriptorSet;

		}
		const VkDescriptorSetLayout& getDescriptorSetLayout() const {
			return m_descriptorLayout->getDescriptorSetLayout();
		}
		void createDescriptors();

	private:
		void createDescriptorLayout();




	private:
		std::unique_ptr<LveDescriptorSetLayout> m_descriptorLayout;
		std::vector<VkDescriptorSet> m_descriptorSet;

		// Buffers GPU
		std::unique_ptr<LveBuffer> m_lightBuffer;
		const LightManager& m_lightManager;

		LveDescriptorPool& m_descriptorPool;
		const LveDevice* m_device;

	};

}