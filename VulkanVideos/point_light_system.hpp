#pragma once

#include "pipeline.hpp"
#include "lve_game_object.hpp"
#include "lve_device.hpp"
#include "lve_camera.hpp"
#include "lve_model.hpp"
#include <memory>
#include <vector>
#include "lve_frame_info.hpp"
#include "lve_descriptor.hpp"
namespace lve {

	class PointLighRenderSystem {

	public:


		PointLighRenderSystem( VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);//globalSetLayout to tell the pipeline what descriptor set layout will be
		~PointLighRenderSystem();

		PointLighRenderSystem(const PointLighRenderSystem&) = delete; // delete copy constructor
		PointLighRenderSystem& operator=(const PointLighRenderSystem&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed
		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);// camera not meber bs we want to be able to share camera object multiple mtiple render system

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);//just to create the pipeline

		//LveWindow _window{ WIDTH,HEIGHT,"SimpleRenderSystem" };// first app created auto creat window destroy auto destroy
		//std::unique_ptr<LveSwapChain> lveSwapChain;//using pointer small performance cost
		std::unique_ptr<LvePipeline> lvePipeline;//smart pointer, avoid memory leak, only one uniqe ptr have the oject dynamcly asscoeied
		VkPipelineLayout pipelineLayout;
		//std::vector<VkCommandBuffer> commandBuffers;
	};
}