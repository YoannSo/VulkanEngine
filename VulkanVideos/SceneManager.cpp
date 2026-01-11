#include "SceneManager.h"

namespace lve {

    SceneManager* SceneManager::s_sceneSingleton = nullptr;

    SceneManager::SceneManager() {
        m_objectMap = Map();
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
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT * 100)
            .build();

		m_materialSystem = std::make_unique<MaterialSystem>(m_textureManager, m_materialManager, *m_descriptorPool);

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

        Assimp::Importer importer;

        const unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

        const aiScene* const scene = importer.ReadFile(p_path, flags);

        if (scene == nullptr)
        {
            std::cout << "FAILED model " << p_meshName << " from: " << p_path << importer.GetErrorString() << std::endl;
            throw std::runtime_error("Fail to load file \" " + p_path + "\": " + importer.GetErrorString());
        }

		m_materialManager.createMaterialsFromMesh(*scene, p_meshName);


        Model* obj = new Model(p_meshName, *scene,m_materialManager);

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


    void SceneManager::addMaterial(std::unique_ptr<Material> p_material) {
        m_materialMap.emplace(p_material->getName(), std::move(p_material));
    }


    void SceneManager::updateAllGameObject(float p_dt) {
        for (auto& gameObject : m_objectMap) {
            gameObject.second->callUpdateFunction(p_dt);
        }
    }

    void SceneManager::initializeMaterialSystem()
    {
        m_materialSystem->setupDescriptorSetLayout();
		m_materialSystem->setupDescriptorSet();
    }

    void SceneManager::setupDescriptorSet() {
        m_objectLocalSetLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();
    }
}
