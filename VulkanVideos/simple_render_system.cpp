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
	SimpleRenderSystem::SimpleRenderSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(LveDevice::getInstance()->getDevice(), pipelineLayout, nullptr);
	}



	void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);
	 
		std::vector<VkDescriptorSetLayout> descriptorSetLayout{ globalSetLayout};

		if (SceneManager::getInstance()->getTextureMap().size() > 0) {
			descriptorSetLayout.push_back(SceneManager::getInstance()->getDescriptorSetLayout().getDescriptorSetLayout());
		}

		descriptorSetLayout.push_back(SceneManager::getInstance()->getMaterialDescriptorSetLayout().getDescriptorSetLayout());

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; //empyt layout so 0 
		pipelineLayoutInfo.setLayoutCount = descriptorSetLayout.size(); //empty so 0
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data(); // pass date, autre que vertex data to vertex and framgnet (texture/uniform)
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // small amount of data, 

		if (vkCreatePipelineLayout(LveDevice::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
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
		lvePipeline = std::make_unique<LvePipeline>("shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv",pipelineConfig);
	}
	


	void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo)
	{
		lvePipeline->bind(frameInfo.commandBuffer);


		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0,//strating sert
			1,//number of set
			&frameInfo.globalDescriptorSet,
			0,
			nullptr
		);


		auto test = SceneManager::getInstance()->getGlobalDescriptorSet();
			vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
				1,//strating sert
				1,//number of set
				&(SceneManager::getInstance()->getGlobalDescriptorSet()[frameInfo.frameIndex]),
				0,
				nullptr
			);
	
			auto renderingBatch = SceneManager::getInstance()->getRenderingBatch();
			auto materialMap = SceneManager::getInstance()->getMaterialMap();

			renderingBatch[0];
			int index=0;
	
			for (auto& test : renderingBatch) {
		auto material = materialMap.find(test.first);

		if (renderingBatch[index].second.size() > 0) {

			vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
				2,//strating sert
				1,//number of set
				&(material->second->getDescriptorSet()[frameInfo.frameIndex]),
				0,
				nullptr
			);

			for (auto& object : renderingBatch[index].second) { // ici changer la fct bind a 
				SimplePushConstantData push{};

				push.modelMatrix = object->transform.mat4();
				push.normalMatrix = object->transform.normalMatrix();

				//record push command date to the command buffer
				vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
				object->bind(frameInfo.commandBuffer, frameInfo.frameIndex, pipelineLayout);
				object->draw(frameInfo.commandBuffer);
			}
		}
		++index;
		}
	}




}

	
