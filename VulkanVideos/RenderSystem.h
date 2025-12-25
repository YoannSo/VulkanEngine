#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include "pipeline.hpp"
#include "GameObject.hpp"
#include "lve_device.hpp"
#include "lve_camera.hpp"
#include "lve_frame_info.hpp"
#include "lve_descriptor.hpp"

namespace lve {

	class RenderSystem {

	public:
		virtual ~RenderSystem();

		RenderSystem(const RenderSystem&) = delete; // delete copy constructor
		RenderSystem& operator=(const RenderSystem&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		virtual std::vector<VkDescriptorSetLayout> buildLayouts(VkDescriptorSetLayout globalSetLayout);

		virtual void update(FrameInfo& frameInfo, GlobalUbo& ubo) {};
		virtual void render(FrameInfo& frameInfo) = 0;// camera not meber bs we want to be able to share camera object multiple mtiple render system
		virtual void createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut);
		void init(VkRenderPass renderPass, std::vector <VkDescriptorSetLayout> p_descriptorSetLayouts, uint32_t p_pushConstantSize, std::string p_vertShader, std::string p_fragShader, bool p_clearAttributes = false);
	
	protected:
		RenderSystem();//globalSetLayout to tell the pipeline what descriptor set layout will be
		void createPipelineLayout(std::vector <VkDescriptorSetLayout> p_descriptorSetLayouts, uint32_t p_pushConstantSize);
		virtual void createPipeline(VkRenderPass renderPass, std::string p_vertShader, std::string p_fragShader, bool p_clearAttributes);//just to create the pipeline

		//LveWindow _window{ WIDTH,HEIGHT,"SimpleRenderSystem" };// first app created auto creat window destroy auto destroy
		//std::unique_ptr<LveSwapChain> lveSwapChain;//using pointer small performance cost
		std::unique_ptr<LvePipeline> lvePipeline;//smart pointer, avoid memory leak, only one uniqe ptr have the oject dynamcly asscoeied
		VkPipelineLayout pipelineLayout;
		//std::vector<VkCommandBuffer> commandBuffers;
	};
}