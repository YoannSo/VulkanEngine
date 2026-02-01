#pragma once
#include <Renderer/RenderSystems/RenderSystem.h>

namespace engine::render_systems
{
	class BillboardRenderSystem :public RenderSystem
	{
		BillboardRenderSystem(VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& p_descriptorSetLayout);
		~BillboardRenderSystem();
	private:
		void createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut) override;
		void render(const FrameInfo& frameInfo) override;

	};

}

