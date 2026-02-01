#include <Engine/Managers/SceneManager.h>

namespace engine {

    SceneManager* SceneManager::s_sceneSingleton = nullptr;

    SceneManager::SceneManager() {
        m_objectMap = Map();
        m_materialMap = MaterialMap();
        m_opaqueRenderingBatch = RenderingBatch();
        m_transparentRenderingBatch = TransparentRenderingBatch();





        m_descriptorPool =
            DescriptorPool::Builder()
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT * 500)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT * MAX_TEXTURE_IN_SCENE)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_OBJECT_IN_SCENE * SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_MATERIAL_IN_SCENE * SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT * 100)
            .build();

        m_globalDescriptorLayout = DescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();



        m_globalUboBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto& uboBuffer : m_globalUboBuffers)
        {
            uboBuffer = std::make_unique<GwatBuffer>(
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffer->map();
        }

		m_globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_globalDescriptorSets.size(); i++) {
            auto bufferInfo = m_globalUboBuffers[i]->descriptorInfo();
            DescriptorWriter(*m_globalDescriptorLayout, *m_descriptorPool)
                .writeBuffer(0, &bufferInfo)
                .build(m_globalDescriptorSets[i]);
        }



		m_materialSystem = std::make_unique<MaterialSystem>(m_textureManager, m_materialManager, *m_descriptorPool);
		m_lightSystem = std::make_unique<LightSystem>(m_lightManager, *m_descriptorPool);

    }

    SceneManager::~SceneManager() {
    }

    Camera* SceneManager::createCameraObject() {
        if (VERBOSE)
            std::cout << "-*-" << " Create Camera Game Object:" << std::endl;

        Camera* obj = new Camera();
        return obj;
    }

    Model* SceneManager::createModelObject(std::string p_meshName, std::string p_path,bool p_useBasicMaterial) {
        if (VERBOSE)
            std::cout << "-*-" << " Create Mesh Game Object:" << p_meshName << std::endl;

        Assimp::Importer importer;

        const unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices| aiProcess_CalcTangentSpace;

        const aiScene* const scene = importer.ReadFile(p_path, flags);

        if (scene == nullptr)
        {
            std::cout << "FAILED model " << p_meshName << " from: " << p_path << importer.GetErrorString() << std::endl;
            throw std::runtime_error("Fail to load file \" " + p_path + "\": " + importer.GetErrorString());
        }

        if(!p_useBasicMaterial)
		    m_materialManager.createMaterialsFromMesh(*scene, p_meshName);


        Model* obj = new Model(p_meshName, *scene,m_materialManager, p_useBasicMaterial);

        m_objectMap.emplace(obj->getId(), obj);
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

    void SceneManager::initializeDescriptor()
    {
		m_materialSystem->setupDescriptorSet();
        m_lightSystem->setupDescriptorSet();
    }



    void SceneManager::writeInGlobalUbo(GlobalUbo& p_buffer, size_t p_frameIndex)
    {
        m_globalUboBuffers[p_frameIndex]->writeToBuffer(&p_buffer);
        m_globalUboBuffers[p_frameIndex]->flush();
    }



}
