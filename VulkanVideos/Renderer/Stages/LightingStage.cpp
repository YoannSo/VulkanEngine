#include "LightingStage.h"


namespace engine::stages {
	LightingStage::LightingStage(const Device& p_device, const SwapChain& m_swapchain, std::shared_ptr<GBuffer> p_gBuffer, const SceneManager& p_sceneManager) : RenderStage(p_device, m_swapchain),
		m_gBuffer(p_gBuffer)
	{
		init();
		std::vector<VkDescriptorSetLayout> descriptorLayoutLightningPass = { p_sceneManager.getGlobalDescriptorSetLayout() };
		m_renderSystems.emplace_back(std::make_unique<LightingPassDeferred>(m_renderPass,descriptorLayoutLightningPass,m_gBuffer));
	}

	void LightingStage::createRenderPass()
	{
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		std::vector<VkSubpassDescription> subpasses;
		std::vector<VkSubpassDependency> dependencies;


		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = m_swapchain.getSwapChainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		attachments.push_back(colorAttachment);

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentRefs.push_back(colorAttachmentRef);

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = colorAttachmentRefs.data();
		subpass.pDepthStencilAttachment = nullptr;

		subpasses.push_back(subpass);

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;



		dependencies.push_back(dependency);


		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = subpasses.data();
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = dependencies.data();


		if (vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create deferred render pass!");
		}
	}

	void LightingStage::createFrameBuffers()
	{
		m_frameBuffers.resize(m_swapchain.imageCount());
		for (size_t i = 0; i < m_swapchain.imageCount(); ++i) {

			std::vector<VkImageView> attachementView;
			//getAttachementView(i, attachementView);

			attachementView.push_back(m_swapchain.getImageView(i));
			//attachementView.push_back(m_swapChain->getDepthImageView(i));

			VkExtent2D swapChainExtent = m_swapchain.getSwapChainExtent();
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachementView.size());
			framebufferInfo.pAttachments = attachementView.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(
				m_device.getDevice(),
				&framebufferInfo,
				nullptr,
				&m_frameBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	std::vector<VkClearValue> LightingStage::getClearValues()
	{
		std::vector<VkClearValue> clearValues(1);
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // color attachment
		return clearValues;
	}
}

