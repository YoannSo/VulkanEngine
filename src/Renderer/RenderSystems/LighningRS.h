#pragma once
#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1

#include <gtc/constants.hpp>
#include <glm.hpp>
#include <memory>
#include <algorithm>

#include <Renderer/RenderSystems/RenderSystem.h>
#include <Renderer/Ressources/GBuffer.hpp>
#include <Engine/Managers/SceneManager.h>

namespace engine {

	class LightingPassDeferred : public RenderSystem {

	public:
		struct SimplePushConstantData {
			uint32_t _lightNumber;
			uint32_t _showNormalMap = 0; // 0: no normal map, 1: normal map
			uint32_t _showLighning = 0; // 0: normal lighting, 1: no lighting (show albedo only)
		};


		LightingPassDeferred(VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& p_descriptorSetLayout, std::shared_ptr<GBuffer> p_gBuffer);//globalSetLayout to tell the pipeline what descriptor set layout will be
		~LightingPassDeferred();

		LightingPassDeferred(const LightingPassDeferred&) = delete; // delete copy constructor
		LightingPassDeferred& operator=(const LightingPassDeferred&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		void update(FrameInfo& frameInfo, SceneManager::GlobalUbo& ubo) override {}
		void render(const FrameInfo& frameInfo)override;// camera not meber bs we want to be able to share camera object multiple mtiple render system

	private:
		void drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs);
		void createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut) override;

		
		std::shared_ptr<GBuffer> m_gBuffer;
		SimplePushConstantData m_pushConstants;
		SceneManager* m_sceneManager = SceneManager::getInstance();
	};
}