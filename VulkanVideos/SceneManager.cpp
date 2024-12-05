#include "SceneManager.h"

namespace lve {



	SceneManager::SceneManager()
	{
		m_objectMap = Map();
		m_textureMap = TextureMap();
		m_shaderTextureId = std::vector<std::string>();


		m_descriptorPool =
			LveDescriptorPool::Builder()
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT*500)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT * MAX_TEXTURE_IN_SCENE)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_OBJECT_IN_SCENE * LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();


		setupDescriptorSet();

	}
	SceneManager::~SceneManager()
	{
	}
	LveGameObject* SceneManager::appendGameObject()
	{
		if (VERBOSE)
			std::cout << "-*-" << " Append Game Object"<< std::endl;
		LveGameObject* newGameObject = LveGameObject::createGameObject();
		m_objectMap.emplace(newGameObject->getId(), newGameObject);
		return newGameObject;
	}
	LveGameObject* SceneManager::createMeshObject(std::string p_meshName, std::string p_path)
	{
		if (VERBOSE)
			std::cout << "-*-" << " Create Mesh Game Object"<<std::endl;

		LveGameObject* obj = LveGameObject::createGameObject();
		obj->_model = std::make_shared<Model>(p_meshName, p_path);
		m_objectMap.emplace(obj->getId(), obj);
		return obj;
	}
	void SceneManager::addGameObject(LveGameObject* p_newGameObject)
	{
		if (VERBOSE)
			std::cout << "-*-" << " add Game Object " << p_newGameObject->getId() << std::endl;
		m_objectMap.emplace(p_newGameObject->getId(), p_newGameObject);
	}
	int SceneManager::addTextureElement(std::string p_path, LveTexture* p_texture)
	{


		m_textureMap.emplace(p_path, p_texture);
		m_shaderTextureId.emplace_back(p_path);

		return m_textureMap.size()-1;
	}

	void SceneManager::setObjectDescriptorSet()
	{
		m_globalSetLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, m_textureMap.size()).build();

		std::cout << "DEBUG " << m_textureMap.size();

			m_globalDescriptorSet = std::vector<VkDescriptorSet>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

			for (int i = 0; i < m_globalDescriptorSet.size(); ++i) {
				std::vector<VkDescriptorImageInfo> imageInfos;
				imageInfos.resize(m_shaderTextureId.size());

				int index = 0;
				for (auto it = m_textureMap.begin(); it != m_textureMap.end(); ++it, ++index) {
					imageInfos[index] = it->second->getDescriptorImageInfo();
				}

				LveDescriptorWriter(*m_globalSetLayout, *m_descriptorPool)
					.writeImages(0, imageInfos)
					.build(m_globalDescriptorSet[i]);

			}
		


	}
	void SceneManager::setupDescriptorSet()
	{
		m_objectLocalSetLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

	}
}
