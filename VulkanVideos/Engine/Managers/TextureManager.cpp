#include "TextureManager.h"

engine::TextureManager::TextureManager()
{
}

engine::TextureManager::~TextureManager()
{
}

uint32_t engine::TextureManager::loadTexture(const std::string& p_texturePath)
{
	if(m_textureMap.find(p_texturePath) != m_textureMap.end()) {
		// Texture already loaded
		return m_textureMap[p_texturePath];
	}

	m_textureMap.emplace(p_texturePath, m_currentIdTexture);

	auto tex = std::make_unique<Texture>(p_texturePath.c_str());
	m_textureArray.emplace_back(std::move(tex));

	return m_currentIdTexture++;//return the id then increment
}

void engine::TextureManager::fillDescriptorImageInfoArray(std::vector<VkDescriptorImageInfo>& p_imageInfos)const
{
	for (auto& tex : m_textureArray) {

		p_imageInfos.emplace_back(tex->getDescriptorImageInfo());
	}
}
