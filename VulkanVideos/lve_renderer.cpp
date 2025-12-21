#include "lve_renderer.hpp"
#include <iostream>
#include <stdexcept>
#include <array>
#include <iostream>
namespace lve {

	LveRenderer::LveRenderer(LveWindow &window): _window(window){
		
		recreateSwapChain();
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
	}

	VkCommandBuffer LveRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call beginframe while already in progress");
  auto result = lveSwapChain->acquireNextImage(&currentImgIndex);

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
		auto commandBuffer=getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImgIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			_window.wasWindowResized()) {
			_window.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
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
		renderPassInfo.renderPass = lveSwapChain->getRenderPass(); // set render pass member swapchain as for us
		renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(currentImgIndex); // which frame buffer this frame buffer ? 

		renderPassInfo.renderArea.offset = { 0,0 }; // render area, 
		renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();//swapchain extent instead of window extent

		std::array<VkClearValue, 2> clearValues{};//what we want the initiale value of frame buffer attachement
		clearValues[0].color = { 0.01f,0.01f,0.01f,1.0f };
		clearValues[1].depthStencil = { 1.f, 0 };//index 0 is color attachement and 1 is deth, because we setuped like this in the render pass

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //record to begin the render pass, VK_SUBPASS_CONTENTS_INLINE=> subsequence will be in the primary command buffer no secondary command buffer, 

			//setup viewport and scissor to swapchain dimensio 
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};

			//set viewport and scissor
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void LveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{	  assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
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
		if (lveSwapChain == nullptr) {
					lveSwapChain = std::make_unique<LveSwapChain>(extent);
		}
		else {
			std::shared_ptr<LveSwapChain> oldSwapChain = std::move(lveSwapChain);
			lveSwapChain = std::make_unique<LveSwapChain>(extent,std::move(lveSwapChain));//allow to create a copy, mem management is not broken
			if (!oldSwapChain->compareSwapFormats(*lveSwapChain.get())) {
				throw std::runtime_error("Swap chain image or depth format has changed");
			}
		
		}

		if(lveSwapChain->renderPassMode== LveSwapChain::RenderPassMode::DEFERRED)
			recreateGBuffer();
		//createPipeline();//technically dont need this, still dependan in the swapchain renderpass, mutiplerender pass work with the pipeleine
	}

	void LveRenderer::recreateGBuffer()
	{
		m_gBuffer = std::make_unique<GBuffer>( lveSwapChain->getSwapChainExtent(),lveSwapChain->imageCount());
		m_gBuffer->createFramebuffers(lveSwapChain->getRenderPass());

	}




	
}