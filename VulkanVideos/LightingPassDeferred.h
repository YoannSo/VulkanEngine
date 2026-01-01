#pragma once
#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1
#include <map>
#include <gtc/constants.hpp>
#include <glm.hpp>
#include <memory>
#include "lve_model.hpp"
#include "RenderSystem.h"
#include "g_buffer.hpp"

namespace lve {

	class LightingPassDeferred : public RenderSystem {

	public:


		LightingPassDeferred(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout p_gBufferDescriptorLayout, std::shared_ptr<GBuffer> p_gBuffer);//globalSetLayout to tell the pipeline what descriptor set layout will be
		~LightingPassDeferred();

		LightingPassDeferred(const LightingPassDeferred&) = delete; // delete copy constructor
		LightingPassDeferred& operator=(const LightingPassDeferred&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		void update(FrameInfo& frameInfo, GlobalUbo& ubo) override {}
		void render(FrameInfo& frameInfo)override;// camera not meber bs we want to be able to share camera object multiple mtiple render system

	private:
		std::vector<VkDescriptorSetLayout> buildLayouts(VkDescriptorSetLayout globalSetLayout);
		void drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs);
		void createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut) override;

		
		std::shared_ptr<GBuffer> m_gBuffer;
	};
}