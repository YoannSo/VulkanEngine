#pragma once
#include <stdio.h>
#include "lve_descriptor.hpp"
#include "lve_device.hpp"
#include <string>
#include <memory>
#include <vector>


#include "lve_texture.hpp"
#include "model.hpp"
#include "Camera.h"
#include "PointLight.h"
#include "define.hpp"

#include "TextureManager.h"
#include "MaterialManager.h"
#include "MaterialSystem.h"
#include "LightManager.h"
#include "LightSystem.h"
namespace lve {

    class SceneManager {
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
        inline LveDescriptorPool& getPool() { return *m_descriptorPool; }

        inline MaterialMap& getMaterialMap() { return m_materialMap; }
		LightManager& getLightManager()  { return m_lightManager; }


        void addMaterial(std::unique_ptr<Material> p_material);

        static SceneManager* s_sceneSingleton;
        static SceneManager* getInstance() {
            if (!s_sceneSingleton)
                s_sceneSingleton = new SceneManager();
            return s_sceneSingleton;
        }
        void updateAllGameObject(float p_dt);

		const Map& getObjectMap() const { return m_objectMap; }

        void initializeMaterialSystem();
		void initializeLightSystem();

        std::vector<VkDescriptorSetLayout> getMaterialSystemDescriptorSetLayouts() const
        {
            std::vector<VkDescriptorSetLayout> layouts;
            m_materialSystem->fillDescriptorSetLayouts(layouts);
            return layouts;
		}

        inline LveDescriptorSetLayout& getLocalDescriptorSetLayout() { return *m_objectLocalSetLayout; }


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

    private:
        void setupDescriptorSet();

    public:
        const uint16_t MAX_TEXTURE_IN_SCENE{ 100 };
        const uint16_t MAX_OBJECT_IN_SCENE{ 100 };
        const uint16_t MAX_MATERIAL_IN_SCENE{ 100 };

    private:
        std::string m_name{ "UNDEFINED" };

        std::unique_ptr<LveDescriptorPool> m_descriptorPool{};
        std::unique_ptr<LveDescriptorSetLayout> m_objectLocalSetLayout;

        Map m_objectMap;
        MaterialMap m_materialMap;
        RenderingBatch m_opaqueRenderingBatch;
        TransparentRenderingBatch m_transparentRenderingBatch;

		TextureManager m_textureManager;
        MaterialManager m_materialManager{ m_textureManager };
		std::unique_ptr<MaterialSystem> m_materialSystem;


        LightManager m_lightManager;
		std::unique_ptr<LightSystem> m_lightSystem;


    };
}
