#pragma once

#include <glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>
#include <memory>

#include <Engine/Managers/BillboardManager.h>
#include<Vulkan/Descriptor.hpp>
#include <Vulkan/Buffer.hpp>
#include <Vulkan/SwapChain.hpp>

namespace engine::systems
{
	using engine::managers::BillboardManager;

	class BillboardSystem
	{

	public:
		BillboardSystem(const BillboardManager& p_billboardManager, DescriptorPool& p_descriptorPool);

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
		const BillboardManager& m_billboardManager;

		std::unique_ptr<DescriptorSetLayout> m_descriptorLayout;
		std::vector<VkDescriptorSet> m_descriptorSet;
		std::unique_ptr<GwatBuffer> m_billboardBuffer;
		DescriptorPool& m_descriptorPool;
		const Device* m_device;

	};
}


