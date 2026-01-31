#include "BillboardRS.h"


namespace engine::render_systems
{
	struct PushConstantData {

	};

	BillboardRenderSystem::BillboardRenderSystem(VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& p_descriptorSetLayout) :RenderSystem() {
		uint32_t pushConstantSize = sizeof(PushConstantData);
		init(renderPass, p_descriptorSetLayout, pushConstantSize, "shaders/billboard.vert.spv", "shaders/billboard.frag.spv", true);
	}

	BillboardRenderSystem::~BillboardRenderSystem()
	{
	}

	void BillboardRenderSystem::createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut)
	{
	}
	void BillboardRenderSystem::render(const FrameInfo& frameInfo)
	{
	}

}



