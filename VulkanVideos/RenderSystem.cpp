#include "RenderSystem.h"

namespace lve {


	RenderSystem::RenderSystem() {

	}

	RenderSystem::~RenderSystem() {
		vkDestroyPipelineLayout(LveDevice::getInstance()->getDevice(), pipelineLayout, nullptr);
	}

	void RenderSystem::init(VkRenderPass renderPass, std::vector <VkDescriptorSetLayout> p_descriptorSetLayouts, uint32_t p_pushConstantSize, std::string p_vertShader, std::string p_fragShader, bool p_clearAttributes) {
		createPipelineLayout(p_descriptorSetLayouts, p_pushConstantSize);
		createPipeline(renderPass, p_vertShader, p_fragShader, p_clearAttributes);
	}



	void RenderSystem::createPipelineLayout(std::vector <VkDescriptorSetLayout> p_descriptorSetLayouts,uint32_t p_pushConstantSize)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = p_pushConstantSize;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; //empyt layout so 0 
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(p_descriptorSetLayouts.size()); //empty so 0
		pipelineLayoutInfo.pSetLayouts = p_descriptorSetLayouts.data(); // pass date, autre que vertex data to vertex and framgnet (texture/uniform)
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // small amount of data, 

		if (vkCreatePipelineLayout(LveDevice::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void RenderSystem::createPipeline(VkRenderPass renderPass, std::string p_vertShader, std::string p_fragShader,bool p_clearAttributes)
	{

		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		createPipelineInfo(pipelineConfig);

		if (p_clearAttributes) {
			pipelineConfig.attributDescriptions.clear();
			pipelineConfig.bindingDescriptions.clear();
		}

		// take swap chain w and h bc dont necessarly match the window,
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(p_vertShader, p_fragShader, pipelineConfig);
	}



	std::vector<VkDescriptorSetLayout> RenderSystem::buildLayouts(VkDescriptorSetLayout globalSetLayout) {

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		//if (SceneManager::getInstance()->getTextureMap().size() > 0) {
		//	descriptorSetLayouts.push_back(SceneManager::getInstance()->getDescriptorSetLayout().getDescriptorSetLayout());
		//}

		descriptorSetLayouts.push_back(SceneManager::getInstance()->getMaterialDescriptorSetLayout().getDescriptorSetLayout());



		return descriptorSetLayouts;
	}

	void RenderSystem::createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut) {
		LvePipeline::defaultFowardPipelineConfigInfo(p_pipelineInfoOut);
		LvePipeline::enableAlphaBlinding(p_pipelineInfoOut);
	}

}