#pragma once
#include <unordered_map>
#include <string>
#include <memory>

#include "Texture.hpp"

namespace engine {

    class TextureManager {
    public:
        using TextureStringToIDMap = std::unordered_map<std::string, uint32_t>; // efficiently look up object

    public:
        TextureManager();
        ~TextureManager();

		uint32_t loadTexture(const std::string& p_texturePath);
        uint32_t getTextureCount()const  { return m_currentIdTexture; }
		void fillDescriptorImageInfoArray(std::vector<VkDescriptorImageInfo>& p_imageInfos)const;


    private:
        void setupDescriptorSet();

    public:
        const uint16_t MAX_TEXTURE_IN_SCENE{ 100 };
        const uint16_t MAX_OBJECT_IN_SCENE{ 100 };
        const uint16_t MAX_MATERIAL_IN_SCENE{ 100 };

        uint32_t m_currentIdTexture{ 0 };

    private:
        TextureStringToIDMap m_textureMap;
		std::vector<std::unique_ptr<Texture>> m_textureArray;
    };
}
