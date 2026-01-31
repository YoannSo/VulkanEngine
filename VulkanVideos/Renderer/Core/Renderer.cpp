#include "Renderer.hpp"

namespace engine {

	Renderer::Renderer(Window& window, Device* p_deviceRef) : _window(window), m_deviceRef(p_deviceRef) {
		m_sceneManagerRef = SceneManager::getInstance();

	}

	void Renderer::init() {

		recreateSwapChain();
		onSwapChainRecreated();
		createCommandBuffers();

	}


	Renderer::~Renderer() {
		freeCommandBuffers();//renderer destroy but app will continue so freecomand buffer

	}

	VkCommandBuffer Renderer::beginFrame()
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

	void Renderer::endFrame()
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
		currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}




	void Renderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(Device::getInstance()->getDevice(), Device::getInstance()->getCommandPool(), static_cast<float>(commandBuffers.size()), commandBuffers.data());
	}

	void Renderer::createCommandBuffers() {
		commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = Device::getInstance()->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(Device::getInstance()->getDevice(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}


	void Renderer::recreateSwapChain()
	{
		auto extent = _window.getExtent();
		while (extent.width == 0 || extent.height == 0) {// if one dimension is sizeless program will pause
			extent = _window.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(Device::getInstance()->getDevice());//make wait the swap chain while we are creating one more
		if (m_swapChain == nullptr) {
			m_swapChain = std::make_unique<SwapChain>(extent);
		}
		else {
			std::shared_ptr<SwapChain> oldSwapChain = std::move(m_swapChain);
			m_swapChain = std::make_unique<SwapChain>(extent, std::move(m_swapChain));//allow to create a copy, mem management is not broken
			if (!m_swapChain->compareSwapFormats(*m_swapChain.get())) {
				throw std::runtime_error("Swap chain image or depth format has changed");
			}

		}

		//if(m_swapChain->renderPassMode== SwapChain::RenderPassMode::DEFERRED)
			//recreateGBuffer();
		//createPipeline();//technically dont need this, still dependan in the swapchain renderpass, mutiplerender pass work with the pipeleine
	}


	void engine::Renderer::render(const FrameInfo& frameInfo) {
		for (auto& stage : m_renderStages) {
			stage->record(frameInfo, currentImgIndex);
		}
	}
}




