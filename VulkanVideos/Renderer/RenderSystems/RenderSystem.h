#pragma once

#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1

#include <memory>
#include <vector>
#include <stdexcept>

#include "Pipeline.hpp"
#include "Device.hpp"
#include "FrameInfo.hpp"

namespace engine {

	class RenderSystem {

	public:
		virtual ~RenderSystem();

		RenderSystem(const RenderSystem&) = delete; // delete copy constructor
		RenderSystem& operator=(const RenderSystem&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		virtual void update(FrameInfo& frameInfo, SceneManager::GlobalUbo& ubo) {};
		virtual void render(const FrameInfo& frameInfo) = 0;// camera not meber bs we want to be able to share camera object multiple mtiple render system
		virtual void createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut);
		void init(VkRenderPass renderPass, std::vector <VkDescriptorSetLayout> p_descriptorSetLayouts, uint32_t p_pushConstantSize, std::string p_vertShader, std::string p_fragShader, bool p_clearAttributes = false);
	
	protected:
		RenderSystem();//globalSetLayout to tell the pipeline what descriptor set layout will be
		void createPipelineLayout(std::vector <VkDescriptorSetLayout> p_descriptorSetLayouts, uint32_t p_pushConstantSize);
		virtual void createPipeline(VkRenderPass renderPass, std::string p_vertShader, std::string p_fragShader, bool p_clearAttributes);//just to create the pipeline

		std::unique_ptr<Pipeline> m_pipeline;//smart pointer, avoid memory leak, only one uniqe ptr have the oject dynamcly asscoeied
		VkPipelineLayout pipelineLayout;
	};
}