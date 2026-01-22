#pragma once
#include <unordered_map>
#include <string>
#include <assimp/scene.h>
#include <iostream>

#include "Material.h"
#include "define.hpp"
#include "TextureManager.h"

namespace lve {

    class MaterialManager {
        using MaterialIDMap = std::unordered_map<std::string, uint32_t>; // efficiently look up object

    public:
        MaterialManager(TextureManager& p_textureManager);
        ~MaterialManager();

		uint32_t createMaterialsFromMesh(const aiScene& p_scene,std::string p_name);
		const MaterialIDMap& getMaterialMap() const { return m_materialNameToID; }
		const std::vector<Material>& getMaterialBuffer() const { return m_materialBuffer; }
		uint32_t getMaterialCount()const { return m_currentIdMaterial; }
		void fillMaterialsBufferData(std::vector<Material::MaterialGPU>& p_materialsBufferData)const;
        uint32_t getIDOfMaterial(const std::string& p_materialName) const;


    private:
		void createMaterial(const aiMaterial* p_mtl);
        void createBasicMaterial();

    public:
        const uint16_t MAX_TEXTURE_IN_SCENE{ 100 };
        const uint16_t MAX_OBJECT_IN_SCENE{ 100 };
        const uint16_t MAX_MATERIAL_IN_SCENE{ 100 };

        uint32_t m_currentIdMaterial{ 0 };

    private:
        MaterialIDMap m_materialNameToID;
        std::vector<Material> m_materialBuffer;		
        TextureManager& m_textureManager;
		std::string _dirPath{ "" };

    };
}
