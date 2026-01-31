#include "RenderStage.h"

namespace engine::stages {

	void RenderStage::init()
	{
		createRenderPass();
		createFrameBuffers();
	}
	void RenderStage::beginRenderPass(const FrameInfo& p_frameInfo, int p_currentSwapchainImage)
	{
		assert(p_frameInfo.commandBuffer != VK_NULL_HANDLE && "Can't begin render pass on null command buffer");
		VkRenderPassBeginInfo renderPassInfo{};

		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = m_frameBuffers[p_currentSwapchainImage];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_swapchain.getSwapChainExtent();

		auto clearValues = getClearValues();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();


		vkCmdBeginRenderPass(p_frameInfo.commandBuffer , &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//setup viewport and scissor to swapchain dimensio 
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swapchain.getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_swapchain.getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_swapchain.getSwapChainExtent() };

		//set viewport and scissor
		vkCmdSetViewport(p_frameInfo.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(p_frameInfo.commandBuffer, 0, 1, &scissor);
	}

	void RenderStage::endRenderPass(const FrameInfo& p_frameInfo)
	{
		vkCmdEndRenderPass(p_frameInfo.commandBuffer);
	}
	void RenderStage::record(const FrameInfo& p_frameInfo, int p_currentSwapchainImage)
	{
		beginRenderPass(p_frameInfo, p_currentSwapchainImage);
		for (auto& renderSystem : m_renderSystems) {
			renderSystem->render(p_frameInfo);
		}
		endRenderPass(p_frameInfo);
	}
}
