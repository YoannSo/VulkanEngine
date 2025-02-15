#pragma once
#include <stdio.h>
#include "lve_descriptor.hpp"
#include "lve_device.hpp"
#include <string>
#include <memory>
#include <vector>
#include "lve_texture.hpp"

#include "model.hpp"
#include "GameObject.hpp"
#include "Camera.h"

#include "define.hpp"
namespace lve {

	class SceneManager
	{
		using TextureMap = std::unordered_map<std::string, LveTexture*>; //effienclty look up object
		using Map = std::unordered_map<uint32_t, GameObject*>; //effienclty look up object
		using MaterialMap = std::unordered_map<std::string, std::shared_ptr<Material>>; //effienclty look up object
		using RenderingBatch= std::vector<std::pair<std::string,std::vector<TriangleMesh*>>>;

	public:
		SceneManager();
		~SceneManager();

		GameObject* appendGameObject();
		Model* createModelObject(std::string p_meshName, std::string p_path);
		Camera* createCameraObject();

		void addGameObject(GameObject* p_newGameObject);

		void loadGameObject(std::string p_name,std::string p_path);

		inline Map getSceneObjects() { return m_objectMap; }
		inline const TextureMap getTextureMap() { return m_textureMap; }
		inline const std::vector<std::string> getShaderTextureId() { return m_shaderTextureId; }

		inline  std::vector<VkDescriptorSet> getGlobalDescriptorSet() { return m_globalDescriptorSet; }
		
		inline LveDescriptorSetLayout& getDescriptorSetLayout() { return *m_globalSetLayout; };

		inline LveDescriptorSetLayout& getLocalDescriptorSetLayout() { return *m_objectLocalSetLayout; };
		inline LveDescriptorSetLayout& getMaterialDescriptorSetLayout() { return *m_materialSetLayout; };

		inline LveDescriptorPool& getPool() { return *m_descriptorPool; };

		inline RenderingBatch& getRenderingBatch() { return m_renderingBatch; };
		inline MaterialMap& getMaterialMap() { return m_materialMap; };


		int addTextureElement(std::string p_path, LveTexture* p_texture);
		void addMaterial(std::unique_ptr<Material> p_material);


		static SceneManager* s_sceneSingleton;
		static SceneManager* getInstance() {
			if (!s_sceneSingleton)
				s_sceneSingleton = new SceneManager();
			return s_sceneSingleton;

		}

		void setMaterialDescriptorSet();
		void setupRenderingBatch();

		void updateAllGameObject(float p_dt);


	private:
		void setupDescriptorSet();



	public:
		const uint16_t MAX_TEXTURE_IN_SCENE{ 100 };
		const uint16_t MAX_OBJECT_IN_SCENE{ 100 };
		const uint16_t MAX_MATERIAL_IN_SCENE{ 100 };

		uint32_t _currentIdTexture{ 0 };
	private:




		std::string m_name{ "UNDEFINED" };
		uint32_t m_nbModel{ 0 };

		//std::unique_ptr<LveDescriptorPool> m_globalModelDescriptorPool;
		std::unique_ptr<LveDescriptorPool> m_descriptorPool{};

		std::unique_ptr <LveDescriptorSetLayout> m_globalSetLayout;
		std::unique_ptr <LveDescriptorSetLayout> m_materialSetLayout;

		std::unique_ptr <LveDescriptorSetLayout> m_objectLocalSetLayout;
		std::vector<VkDescriptorSet> m_globalDescriptorSet;



		TextureMap m_textureMap;
		Map m_objectMap;
		MaterialMap m_materialMap;
		RenderingBatch m_renderingBatch;
		std::vector<std::string> m_shaderTextureId;




	};
}

