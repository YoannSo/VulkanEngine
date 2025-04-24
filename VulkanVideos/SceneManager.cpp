#include "SceneManager.h"

namespace lve {

    SceneManager* SceneManager::s_sceneSingleton = nullptr;

    SceneManager::SceneManager() {
        m_objectMap = Map();
        m_textureMap = TextureMap();
        m_materialMap = MaterialMap();
        m_opaqueRenderingBatch = RenderingBatch();
        m_transparentRenderingBatch = TransparentRenderingBatch();
        m_lightMap = LightMap();
        m_shaderTextureId = std::vector<std::string>();

        m_descriptorPool =
            LveDescriptorPool::Builder()
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT * 500)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT * MAX_TEXTURE_IN_SCENE)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_OBJECT_IN_SCENE * LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_MATERIAL_IN_SCENE * LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        setupDescriptorSet();
    }

    SceneManager::~SceneManager() {
    }

    Camera* SceneManager::createCameraObject() {
        if (VERBOSE)
            std::cout << "-*-" << " Create Camera Game Object:" << std::endl;

        Camera* obj = new Camera();
        return obj;
    }

    Model* SceneManager::createModelObject(std::string p_meshName, std::string p_path) {
        if (VERBOSE)
            std::cout << "-*-" << " Create Mesh Game Object:" << p_meshName << std::endl;

        Model* obj = new Model(p_meshName, p_path);
        m_objectMap.emplace(obj->getId(), obj);
        return obj;
    }

    PointLight* SceneManager::createLightObject() {
        if (VERBOSE)
            std::cout << "-*-" << " Create Light Game Object" << std::endl;

        PointLight* obj = new PointLight();
        m_objectMap.emplace(obj->getId(), obj);
        m_lightMap.emplace_back(obj);
        return obj;
    }

    void SceneManager::addGameObject(GameObject* p_newGameObject) {
        if (VERBOSE)
            std::cout << "-*-" << " add Game Object " << p_newGameObject->getId() << std::endl;
        m_objectMap.emplace(p_newGameObject->getId(), p_newGameObject);
    }

    int SceneManager::addTextureElement(std::string p_path, LveTexture* p_texture) {
        m_textureMap.emplace(p_path, p_texture);
        m_shaderTextureId.emplace_back(p_path);
        return m_textureMap.size() - 1;
    }

    void SceneManager::addMaterial(std::unique_ptr<Material> p_material) {
        m_materialMap.emplace(p_material->getName(), std::move(p_material));
    }

    void SceneManager::setMaterialDescriptorSet() {
        m_globalSetLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, _currentIdTexture).build();

        for (auto it = m_materialMap.begin(); it != m_materialMap.end(); ++it) {
            it->second->setupDescriptorSet();
        }

        m_globalDescriptorSet = std::vector<VkDescriptorSet>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < m_globalDescriptorSet.size(); ++i) {
            std::vector<VkDescriptorImageInfo> imageInfos;

            int index = 0;

            for (auto it = m_materialMap.begin(); it != m_materialMap.end(); ++it, ++index) {
                std::vector<VkDescriptorImageInfo> getImageInfo = it->second->getVkDescriptorImages();
                if (getImageInfo.size() > 0)
                    imageInfos.insert(imageInfos.end(), std::make_move_iterator(getImageInfo.begin()), std::make_move_iterator(getImageInfo.end()));
            }

            if (imageInfos.size() > 0)
                LveDescriptorWriter(*m_globalSetLayout, *m_descriptorPool)
                    .writeImages(0, imageInfos)
                    .build(m_globalDescriptorSet[i]);
            else
                LveDescriptorWriter(*m_globalSetLayout, *m_descriptorPool)
                .build(m_globalDescriptorSet[i]);
        }
    }

    void SceneManager::setupRenderingBatch() {
        for (auto& material : m_materialMap) {
            const std::string name = material.first;
            for (auto& gameObject : m_objectMap) {
                if (Model* model = dynamic_cast<Model*>(gameObject.second)) {
                    if (material.second->isTransparent()) {
                        std::vector<lve::TriangleMesh*> meshes = model->getAllMeshesFromMaterial(name);
                        for (auto& mesh : meshes)
                            m_transparentRenderingBatch.emplace_back(std::make_pair(name, mesh));
                    }
                    else {
                        m_opaqueRenderingBatch.emplace_back(std::make_pair(name, model->getAllMeshesFromMaterial(name)));
                    }
                }
            }
        }
    }

    void SceneManager::updateAllGameObject(float p_dt) {
        for (auto& gameObject : m_objectMap) {
            gameObject.second->callUpdateFunction(p_dt);
        }
    }

    void SceneManager::setupDescriptorSet() {
        m_objectLocalSetLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();
        m_materialSetLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    }
}
