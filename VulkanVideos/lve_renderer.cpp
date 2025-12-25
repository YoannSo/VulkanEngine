#include "lve_renderer.hpp"
#include <iostream>
#include <stdexcept>
#include <array>
#include <iostream>
namespace lve {

	LveRenderer::LveRenderer(LveWindow& window, LveDevice* p_deviceRef) : _window(window), m_deviceRef(p_deviceRef) {


	}

	void LveRenderer::init() {
		recreateSwapChain();
		onSwapChainRecreated();
		createRenderPass();
		createFramebuffers();
		createCommandBuffers();
	}
	void LveRenderer::writeTimestampStart(VkCommandBuffer cmd, VkQueryPool queryPool, uint32_t index) {
		if (queryPool == VK_NULL_HANDLE) return;
		vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, queryPool, index);
	}

	void LveRenderer::writeTimestampEnd(VkCommandBuffer cmd, VkQueryPool queryPool, uint32_t index) {
		if (queryPool == VK_NULL_HANDLE) return;
		vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, queryPool, index);
	}
	LveRenderer::~LveRenderer() {
		freeCommandBuffers();//renderer destroy but app will continue so freecomand buffer

		for (auto framebuffer : m_frameBuffers) {
			vkDestroyFramebuffer(m_deviceRef->getDevice(), framebuffer, nullptr);
		}

		vkDestroyRenderPass(m_deviceRef->getDevice(), m_renderPass, nullptr);

	}

	VkCommandBuffer LveRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call beginframe while already in progress");
		auto result = m_swapChain->acquireNextImage(&currentImgIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;//indicate that the frame not succefly started
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}
		return commandBuffer;

	}

	void LveRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call beginFrame while already in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = m_swapChain->submitCommandBuffers(&commandBuffer, &currentImgIndex);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			_window.wasWindowResized()) {
			_window.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void LveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame");
		VkRenderPassBeginInfo renderPassInfo{}; //first command begin render pass
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass; // set render pass member swapchain as for us
		renderPassInfo.framebuffer = m_frameBuffers[currentImgIndex]; // which frame buffer this frame buffer ? 

		renderPassInfo.renderArea.offset = { 0,0 }; // render area, 
		renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();//swapchain extent instead of window extent
			
		std::array<VkClearValue, 4> clearValues{};//what we want the initiale value of frame buffer attachement
		clearValues[0].color = { 0.0f,0.0f,0.0f,1.0f };//albedo attachement
		clearValues[2].color = { 0.0f,0.0f,0.0f,1.0f };//normal attachement
		clearValues[3].color = { 0.0f,0.0f,0.0f,1.0f };//position attachement
		clearValues[1].depthStencil = { 1.f, 0 };//index 0 is color attachement and 1 is deth, because we setuped like this in the render pass

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //record to begin the render pass, VK_SUBPASS_CONTENTS_INLINE=> subsequence will be in the primary command buffer no secondary command buffer, 

		//setup viewport and scissor to swapchain dimensio 
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_swapChain->getSwapChainExtent() };

		//set viewport and scissor
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void LveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);

	}

		
	void LveRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(LveDevice::getInstance()->getDevice(), LveDevice::getInstance()->getCommandPool(), static_cast<float>(commandBuffers.size()), commandBuffers.data());
	}

	void LveRenderer::createCommandBuffers() {
		commandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = LveDevice::getInstance()->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(LveDevice::getInstance()->getDevice(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}


	void LveRenderer::recreateSwapChain()
	{
		auto extent = _window.getExtent();
		while (extent.width == 0 || extent.height == 0) {// if one dimension is sizeless program will pause
			extent = _window.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(LveDevice::getInstance()->getDevice());//make wait the swap chain while we are creating one more
		if (m_swapChain == nullptr) {
			m_swapChain = std::make_unique<LveSwapChain>(extent);
		}
		else {
			std::shared_ptr<LveSwapChain> oldSwapChain = std::move(m_swapChain);
			m_swapChain = std::make_unique<LveSwapChain>(extent, std::move(m_swapChain));//allow to create a copy, mem management is not broken
			if (!m_swapChain->compareSwapFormats(*m_swapChain.get())) {
				throw std::runtime_error("Swap chain image or depth format has changed");
			}

		}

		//if(m_swapChain->renderPassMode== LveSwapChain::RenderPassMode::DEFERRED)
			//recreateGBuffer();
		//createPipeline();//technically dont need this, still dependan in the swapchain renderpass, mutiplerender pass work with the pipeleine
	}

	void LveRenderer::createFramebuffers() {
		m_frameBuffers.resize(m_swapChain->imageCount());
		for (size_t i = 0; i < m_swapChain->imageCount(); ++i) {
			
			std::vector<VkImageView> attachementView;
			getAttachementView(i, attachementView);

			VkExtent2D swapChainExtent = m_swapChain->getSwapChainExtent();
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachementView.size());
			framebufferInfo.pAttachments = attachementView.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(
				m_deviceRef->getDevice(),
				&framebufferInfo,
				nullptr,
				&m_frameBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}



	void LveRenderer::createRenderPass() {

		fillRenderPassInfo();// virtual fction implemented in derived class

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
		renderPassInfo.pAttachments = m_attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = m_subpasses.data();
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = m_dependencies.data();


		if (vkCreateRenderPass(m_deviceRef->getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create deferred render pass!");
		}
	}


}
void lve::LveRenderer::updateRenderSystems(FrameInfo& frameInfo, GlobalUbo& ubo)
{
	for (auto& renderSystem : m_renderSystems) {
		renderSystem->update(frameInfo, ubo);
	}
}
void lve::LveRenderer::renderRenderSystems(FrameInfo& frameInfo)
{
	for (auto& renderSystem : m_renderSystems) {
		renderSystem->render(frameInfo);

	}
}


