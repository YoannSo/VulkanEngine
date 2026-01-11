#pragma once

#include <glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

#include "lve_buffer.hpp"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "lve_descriptor.hpp"
#include "lve_swap_chain.hpp"
namespace lve
{


	class MaterialSystem
	{

	
	public:
		MaterialSystem(const TextureManager& p_textureManager, const MaterialManager& p_materialManager,LveDescriptorPool& p_descriptorPool);
		~MaterialSystem();


		void setupDescriptorSetLayout();
		void setupDescriptorSet();

		void fillDescriptorSetLayouts(std::vector<VkDescriptorSetLayout>& p_layouts) const
		{
			p_layouts.push_back(m_bindlessTextureLayout->getDescriptorSetLayout());
			p_layouts.push_back(m_materialLayout->getDescriptorSetLayout());
		}

		const std::vector<VkDescriptorSet>& getBindlessTextureSet() const { 
			
			return m_bindlessTextureSet; 
		
		}
		const std::vector<VkDescriptorSet>& getMaterialDescriptorSet() const { 
			
			return m_materialDescriptorSets;
		
		}

	private:
		void createTextureDescriptors();
		void createMaterialDescriptors();
		void createTextureDescriptorLayout();
		void createMaterialDescriptorLayout();




	private:
		// Descriptors + layouts
		std::unique_ptr<LveDescriptorSetLayout> m_bindlessTextureLayout;
		std::vector<VkDescriptorSet> m_bindlessTextureSet;

		std::unique_ptr<LveDescriptorSetLayout> m_materialLayout;
		std::vector<VkDescriptorSet> m_materialDescriptorSets;

		// Buffers GPU
		std::unique_ptr<LveBuffer> m_materialBuffer;

		// Référence au TextureManager pour créer bindless descriptors
		const TextureManager& m_textureManager;
		const MaterialManager& m_materialManager;

		LveDescriptorPool& m_descriptorPool;

		const LveDevice* m_device;

	};

}