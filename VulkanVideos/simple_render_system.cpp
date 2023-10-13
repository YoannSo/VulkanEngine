#include "simple_render_system.hpp"
#include <stdexcept>
#include <array>
#include <glm/gtc/constants.hpp>
#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1
#include <glm/glm.hpp>
namespace lve {
	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.f };//offest homegous 
		glm::mat4 normalMatrix{ 1.f };
	};
	SimpleRenderSystem::SimpleRenderSystem(LveDevice& device,VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& globalSetLayout): lveDevice(device) {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}



	void SimpleRenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout>& globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


	//	std::vector<VkDescriptorSetLayout> descriptorSetLayout{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; //empyt layout so 0 
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(globalSetLayout.size()); //empty so 0
		pipelineLayoutInfo.pSetLayouts = globalSetLayout.data(); // pass date, autre que vertex data to vertex and framgnet (texture/uniform)
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // small amount of data, 

		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
      VK_SUCCESS) {
		 throw std::runtime_error("failed to create pipeline layout!");
		 }
	}
	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{

		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfig{};
  LvePipeline::defaultPipelineConfigInfo(
      pipelineConfig);
  // take swap chain w and h bc dont necessarly match the window,
  pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv",pipelineConfig);
	}
	


void SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo)
{
	lvePipeline->bind(frameInfo.commandBuffer);


	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 
		0,//strating sert
		1,//number of set
		&frameInfo.globalDescriptorSet,
		0,
		nullptr
		);

	for (auto& kv : frameInfo.gameObjects) {
		auto& obj = kv.second;
		if (obj._model == nullptr) continue;
		SimplePushConstantData push{};

		push.modelMatrix = obj.transform.mat4();
		push.normalMatrix =obj.transform.normalMatrix();
			//record push command date to the command buffer
			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			//obj._model->bind(frameInfo.commandBuffer,frameInfo.frameIndex,pipelineLayout);
			obj._model->draw(frameInfo.commandBuffer, frameInfo.frameIndex, pipelineLayout);

		
	}


}

	
}