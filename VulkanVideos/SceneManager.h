#pragma once
#include <stdio.h>
#include "lve_descriptor.hpp"
#include "lve_device.hpp"
#include <string>
#include <memory>
#include <vector>
#include "lve_texture.hpp"
#include "model.hpp"
#include "lve_game_object.hpp"
#include "define.hpp"
namespace lve {

	class SceneManager
	{
		using TextureMap = std::unordered_map<std::string, LveTexture*>; //effienclty look up object
		using Map = std::unordered_map<uint32_t, LveGameObject*>; //effienclty look up object


	public:
		SceneManager();
		~SceneManager();

		LveGameObject* appendGameObject();
		LveGameObject* createMeshObject(std::string p_meshName, std::string p_path);

		void addGameObject(LveGameObject* p_newGameObject);

		void loadGameObject(std::string p_name,std::string p_path);

		inline Map getSceneObjects() { return m_objectMap; }
		inline const TextureMap getTextureMap() { return m_textureMap; }
		inline const std::vector<std::string> getShaderTextureId() { return m_shaderTextureId; }

		inline  std::vector<VkDescriptorSet> getGlobalDescriptorSet() { return m_globalDescriptorSet; }
		
		inline LveDescriptorSetLayout& getDescriptorSetLayout() { return *m_globalSetLayout; };

		inline LveDescriptorSetLayout& getLocalDescriptorSetLayout() { return *m_objectLocalSetLayout; };
		inline LveDescriptorPool& getPool() { return *m_descriptorPool; };


		int addTextureElement(std::string p_path, LveTexture* p_texture);


		static SceneManager* s_sceneSingleton;
		static SceneManager* getInstance() {
			if (!s_sceneSingleton)
				s_sceneSingleton = new SceneManager();
			return s_sceneSingleton;

		}

		void setObjectDescriptorSet();


	private:
		void setupDescriptorSet();



	public:
		const uint16_t MAX_TEXTURE_IN_SCENE{ 100 };
		const uint16_t MAX_OBJECT_IN_SCENE{ 100 };
	private:




		std::string m_name{ "UNDEFINED" };
		uint32_t m_nbModel{ 0 };

		//std::unique_ptr<LveDescriptorPool> m_globalModelDescriptorPool;
		std::unique_ptr<LveDescriptorPool> m_descriptorPool{};
		std::unique_ptr <LveDescriptorSetLayout> m_globalSetLayout;
		std::unique_ptr <LveDescriptorSetLayout> m_objectLocalSetLayout;
		std::vector<VkDescriptorSet> m_globalDescriptorSet;




		TextureMap m_textureMap;
		Map m_objectMap;
		std::vector<std::string> m_shaderTextureId;




	};
}

