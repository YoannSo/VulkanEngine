#pragma once

#include <glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

#include "Buffer.hpp"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "Descriptor.hpp"
#include "SwapChain.hpp"
namespace engine
{


	class MaterialSystem
	{

	
	public:
		MaterialSystem(const TextureManager& p_textureManager, const MaterialManager& p_materialManager,DescriptorPool& p_descriptorPool);
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
		std::unique_ptr<DescriptorSetLayout> m_bindlessTextureLayout;
		std::vector<VkDescriptorSet> m_bindlessTextureSet;

		std::unique_ptr<DescriptorSetLayout> m_materialLayout;
		std::vector<VkDescriptorSet> m_materialDescriptorSets;

		// Buffers GPU
		std::unique_ptr<GwatBuffer> m_materialBuffer;

		// Référence au TextureManager pour créer bindless descriptors
		const TextureManager& m_textureManager;
		const MaterialManager& m_materialManager;

		DescriptorPool& m_descriptorPool;

		const Device* m_device;

	};

}