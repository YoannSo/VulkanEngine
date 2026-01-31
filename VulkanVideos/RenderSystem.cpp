#include "RenderSystem.h"

namespace engine {


	RenderSystem::RenderSystem() {

	}

	RenderSystem::~RenderSystem() {
		vkDestroyPipelineLayout(Device::getInstance()->getDevice(), pipelineLayout, nullptr);
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

		if (vkCreatePipelineLayout(Device::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
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
		m_pipeline = std::make_unique<Pipeline>(p_vertShader, p_fragShader, pipelineConfig);
	}

	void RenderSystem::createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut) {
		Pipeline::defaultFowardPipelineConfigInfo(p_pipelineInfoOut);
		Pipeline::enableAlphaBlinding(p_pipelineInfoOut);
	}

}