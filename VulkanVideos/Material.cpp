#include "Material.h"
#include "SceneManager.h"

namespace lve {

	Material::Material(std::string p_name): m_name(p_name)
	{
	}
	void Material::setMaterialParameter(EMaterialParameter p_materialParameter, std::shared_ptr<LveTexture> p_texture) {
		switch (p_materialParameter) {
			case EMaterialParameter::DIFFUSEMAP:
				m_diffuseMap = p_texture;
				m_ubo.idDiffuseMap = SceneManager::getInstance()->_currentIdTexture;
				break;
			case EMaterialParameter::SPECULARMAP:
				m_specularMap = p_texture;
				m_ubo.m_idSpecularMap = SceneManager::getInstance()->_currentIdTexture;
				break;
			case EMaterialParameter::AMBIENTMAP:
				m_ambientMap = p_texture;
				m_ubo.m_idAmbientMap = SceneManager::getInstance()->_currentIdTexture;
				break;
		}
		++SceneManager::getInstance()->_currentIdTexture;
	}
	void Material::setMaterialParameter(EMaterialParameter p_materialParameter, glm::vec4 p_color) {
		switch (p_materialParameter) {
		case EMaterialParameter::DIFFUSECOLOR:
			m_ubo.diffuseColor = p_color;
			break;
		case EMaterialParameter::SPECULARCOLOR:
			m_ubo.m_specular = p_color;
			break;
		}
	}
	std::vector<VkDescriptorImageInfo> getVkDescriptorImages();

	std::vector<VkDescriptorImageInfo> Material::getVkDescriptorImages() {
		std::vector<VkDescriptorImageInfo> imageInfos;

		if (m_diffuseMap)
			imageInfos.emplace_back(m_diffuseMap->getDescriptorImageInfo());
		if (m_normalMap)
			imageInfos.emplace_back(m_normalMap->getDescriptorImageInfo());
		if (m_specularMap)
			imageInfos.emplace_back(m_specularMap->getDescriptorImageInfo());
		if (m_ambientMap)
			imageInfos.emplace_back(m_ambientMap->getDescriptorImageInfo());
		if (m_shininessMap)
			imageInfos.emplace_back(m_shininessMap->getDescriptorImageInfo());
	
		return imageInfos;
	}
	void Material::setupDescriptorSet() {

		m_materialDescriptorSet = std::vector<VkDescriptorSet>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		m_uboBuffer = std::vector<LveBuffer*>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (auto& buffer : m_uboBuffer) {
			buffer = new LveBuffer(sizeof(MaterialUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			buffer->map();
			buffer->writeToBuffer(&m_ubo);
			buffer->flush();
		}

		
		for (int i = 0; i < m_materialDescriptorSet.size(); ++i) {


			auto bufferUboInfo = m_uboBuffer[i]->descriptorInfo();
			LveDescriptorWriter(SceneManager::getInstance()->getMaterialDescriptorSetLayout(), SceneManager::getInstance()->getPool())
				.writeBuffer(0, &bufferUboInfo)
				.build(m_materialDescriptorSet[i]);


		}
	}
}

