#include <Renderer/Stages/ShadowStage.h>

namespace engine::stages {

ShadowStage::ShadowStage(const Device& p_device, const SwapChain& p_swapchain, const SceneManager& p_sceneManager)
    : RenderStage(p_device, p_swapchain)
{ 

	m_shadowMap = std::make_shared<Texture>(p_swapchain.getSwapChainExtent().width, p_swapchain.getSwapChainExtent().height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,true);

    init();
    std::vector<VkDescriptorSetLayout> descriptorLayoutGeometryPass = { p_sceneManager.getGlobalDescriptorSetLayout()};


    m_renderSystems.emplace_back(std::make_unique<ShadowRenderSystem>(m_renderPass, descriptorLayoutGeometryPass));
}

void ShadowStage::createRenderPass()
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    std::array<VkAttachmentDescription, 1> attachments = { depthAttachment };

    VkAttachmentReference depthRef{};
    depthRef.attachment = 0;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 0;
    subpass.pColorAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depthRef;

    // Ensure correct layout/visibility transitions for depth image
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    // From whatever uses the image before (shader read) or previous external operation
    dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    // To depth attachment write
    dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shadow/geometry render pass!");
    }
}

void ShadowStage::createFrameBuffers()
{
    m_frameBuffers.resize(m_swapchain.imageCount());

    for (size_t i = 0; i < m_swapchain.imageCount(); ++i) {
        
        std::vector<VkImageView> attachementView;
		attachementView.push_back(m_shadowMap->getImageView());
        VkExtent2D swapChainExtent = m_swapchain.getSwapChainExtent();
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
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

std::vector<VkClearValue> ShadowStage::getClearValues()
{
    std::vector<VkClearValue> clearValues{1};//what we want the initiale value of frame buffer attachement

    clearValues[0].depthStencil = { 1.0f, 0 }; // depth

    return clearValues;
}

} // namespace engine::stages
