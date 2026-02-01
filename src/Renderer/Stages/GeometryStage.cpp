#include <Renderer/Stages/GeometryStage.h>

namespace engine::stages {

GeometryStage::GeometryStage(const Device& p_device, const SwapChain& p_swapchain, std::shared_ptr<GBuffer> p_gBuffer, const SceneManager& p_sceneManager)
    : RenderStage(p_device, p_swapchain), m_gBuffer(p_gBuffer)
{ 
    init();
    std::vector<VkDescriptorSetLayout> descriptorLayoutGeometryPass = { p_sceneManager.getGlobalDescriptorSetLayout()};

    std::vector<VkDescriptorSetLayout> materialLayouts = p_sceneManager.getMaterialSystemDescriptorSetLayouts();

    descriptorLayoutGeometryPass.insert(descriptorLayoutGeometryPass.end(), materialLayouts.begin(), materialLayouts.end());

    m_renderSystems.emplace_back(std::make_unique<GeometryPassRenderSystem>(m_renderPass, descriptorLayoutGeometryPass));
}

void GeometryStage::createRenderPass()
{

    VkAttachmentDescription albedoAttachment{};
    albedoAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    albedoAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    albedoAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    albedoAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    albedoAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    albedoAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    albedoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription normalAttachment{};
    normalAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;    
    normalAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    normalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    normalAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    normalAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    normalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription positionAttachment{};
    positionAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    positionAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    positionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    positionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    positionAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    positionAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    positionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    positionAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = m_device.findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    std::array<VkAttachmentDescription, 4> attachments = {
        albedoAttachment,
        normalAttachment,
        positionAttachment,
        depthAttachment
    };


    VkAttachmentReference albedoRef{};
    albedoRef.attachment = 0;
    albedoRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference normalRef{};
    normalRef.attachment = 1;
    normalRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference positionRef{};
    positionRef.attachment = 2;
    positionRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::array<VkAttachmentReference, 3> attachementReference = {
    albedoRef,
    normalRef,
    positionRef,
    };

    VkAttachmentReference depthRef{};
    depthRef.attachment = 3;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 3;
    subpass.pColorAttachments = attachementReference.data();
    subpass.pDepthStencilAttachment = &depthRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;


    if (vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create geometry render pass!");
    }
}

void GeometryStage::createFrameBuffers()
{
    m_frameBuffers.resize(m_swapchain.imageCount());
    for (size_t i = 0; i < m_swapchain.imageCount(); ++i) {

        std::vector<VkImageView> attachementView;
        m_gBuffer->getImageViews(i, attachementView);
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

std::vector<VkClearValue> GeometryStage::getClearValues()
{
    std::vector<VkClearValue> clearValues{4};//what we want the initiale value of frame buffer attachement

    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // albedo
    clearValues[1].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // normal
    clearValues[2].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // position
    clearValues[3].depthStencil = { 1.0f, 0 };                 // depth

    return clearValues;
}

} // namespace engine::stages
