#pragma once
#include <stdio.h>
#include "Descriptor.hpp"
#include "Device.hpp"
#include <string>
#include <memory>
#include <vector>


#include "Texture.hpp"
#include "model.hpp"
#include "Camera.hpp"
#include "PointLight.h"
#include "define.hpp"

#include "TextureManager.h"

#include "MaterialManager.h"
#include "MaterialSystem.h"

#include "LightManager.h"
#include "LightSystem.h"

#include "BillboardManager.h"
#include "BillboardSystem.h"

namespace engine {

	using namespace managers;
    using namespace systems;

    class SceneManager {

    public:
        struct GlobalUbo {
            glm::mat4 projection{ 1.f };
            glm::mat4 view{ 1.f };
            glm::mat4 inverseView{ 1.f };//cam pos last colum
        };


    public:
        using Map = std::unordered_map<uint32_t, GameObject*>; // efficiently look up object
        using RenderingBatch = std::vector<std::pair<std::string, std::vector<TriangleMesh*>>>;
        using TransparentRenderingBatch = std::vector<std::pair<std::string, TriangleMesh*>>;
        using MaterialMap = std::unordered_map<std::string, std::shared_ptr<Material>>; // efficiently look up object

    public:
        SceneManager();
        ~SceneManager();

        Model* createModelObject(std::string p_meshName, std::string p_path,bool p_useBasicMaterial=false);
        Camera* createCameraObject();
        void addGameObject(GameObject* p_newGameObject);

        inline Map getSceneObjects() { return m_objectMap; }
        inline DescriptorPool& getPool() { return *m_descriptorPool; }

        inline MaterialMap& getMaterialMap() { return m_materialMap; }
		LightManager& getLightManager()  { return m_lightManager; }
		BillboardManager& getBillboardManager() { return m_billboardManager; }


        void addMaterial(std::unique_ptr<Material> p_material);

        static SceneManager* s_sceneSingleton;
        static SceneManager* getInstance() {
            if (!s_sceneSingleton)
                s_sceneSingleton = new SceneManager();
            return s_sceneSingleton;
        }
        void updateAllGameObject(float p_dt);

		const Map& getObjectMap() const { return m_objectMap; }

        void initializeDescriptor();

        std::vector<VkDescriptorSetLayout> getMaterialSystemDescriptorSetLayouts() const
        {
            std::vector<VkDescriptorSetLayout> layouts;
            m_materialSystem->fillDescriptorSetLayouts(layouts);
            return layouts;
		}


        const std::vector < VkDescriptorSet>& getMaterialSystemBindlessTextureSet() const
        {

            return m_materialSystem->getBindlessTextureSet();
		}

        const std::vector < VkDescriptorSet>& getMaterialSystemMaterialSet() const
        {

            return m_materialSystem->getMaterialDescriptorSet();
        }

        const std::vector < VkDescriptorSet>& getLightDescriptorSet() const
        {

            return m_lightSystem->getDescriptorSet();
        }
        const VkDescriptorSetLayout& getLightDescriptorSetLayout() const
        {

            return m_lightSystem->getDescriptorSetLayout();
        }


        const VkDescriptorSetLayout& getGlobalDescriptorSetLayout() const
        {
            return m_globalDescriptorLayout->getDescriptorSetLayout();
		}

        const std::vector<VkDescriptorSet>& getGlobalDescriptorSets() const
        {
            return m_globalDescriptorSets;
        }

        void writeInGlobalUbo(GlobalUbo& p_buffer, size_t p_frameIndex);

    public:
        const uint16_t MAX_TEXTURE_IN_SCENE{ 100 };
        const uint16_t MAX_OBJECT_IN_SCENE{ 100 };
        const uint16_t MAX_MATERIAL_IN_SCENE{ 100 };

		bool _showNormal = false;
		bool _showLighning = true;

    private:
        std::string m_name{ "UNDEFINED" };

        std::unique_ptr<DescriptorPool> m_descriptorPool{};
        std::unique_ptr<DescriptorSetLayout> m_globalDescriptorLayout;
        std::vector<std::unique_ptr<GwatBuffer>> m_globalUboBuffers;
		std::vector<VkDescriptorSet> m_globalDescriptorSets;


        Map m_objectMap;
        MaterialMap m_materialMap;
        RenderingBatch m_opaqueRenderingBatch;
        TransparentRenderingBatch m_transparentRenderingBatch;

		TextureManager m_textureManager;
        MaterialManager m_materialManager{ m_textureManager };
		std::unique_ptr<MaterialSystem> m_materialSystem;


        LightManager m_lightManager;
		std::unique_ptr<LightSystem> m_lightSystem;

        BillboardManager m_billboardManager;
		std::unique_ptr<BillboardSystem> m_billboardSystem;





    };
}
