#include "TextureManager.h"

lve::TextureManager::TextureManager()
{
}

lve::TextureManager::~TextureManager()
{
}

uint32_t lve::TextureManager::loadTexture(const std::string& p_texturePath)
{
	if(m_textureMap.find(p_texturePath) != m_textureMap.end()) {
		// Texture already loaded
		return m_textureMap[p_texturePath];
	}

	m_textureMap.emplace(p_texturePath, m_currentIdTexture);

	auto tex = std::make_unique<LveTexture>(p_texturePath.c_str());
	m_textureArray.emplace_back(std::move(tex));

	return m_currentIdTexture++;//return the id then increment
}

void lve::TextureManager::fillDescriptorImageInfoArray(std::vector<VkDescriptorImageInfo>& p_imageInfos)const
{
	for (auto& tex : m_textureArray) {

		p_imageInfos.emplace_back(tex->getDescriptorImageInfo());
	}
}
