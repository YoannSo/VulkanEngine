#include "DeferredRenderer.hpp"

lve::DeferredRenderer::DeferredRenderer(LveWindow& window, LveDevice* p_deviceRef) :LveRenderer(window, p_deviceRef)
{
    init();
}

lve::DeferredRenderer::~DeferredRenderer()
{
}

void lve::DeferredRenderer::createRenderSystems(VkDescriptorSetLayout p_globalDescriptorSet)
{

	m_renderSystems.push_back(std::make_unique<GeometryPassRenderSystem>(m_renderPass, p_globalDescriptorSet));

}

void lve::DeferredRenderer::fillRenderPassInfo()
{

    m_attachments.clear();
    m_subpasses.clear();
    m_dependencies.clear();
    m_colorAttachmentRefs.clear();

    VkAttachmentDescription albedoAttachment{};
    albedoAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    albedoAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    albedoAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    albedoAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    albedoAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    albedoAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    albedoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_attachments.push_back(albedoAttachment);

    VkAttachmentDescription normalAttachment{};
    normalAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    normalAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    normalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    normalAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    normalAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    normalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_attachments.push_back(normalAttachment);

    VkAttachmentDescription positionAttachment{};
    positionAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    positionAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    positionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    positionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    positionAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    positionAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    positionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    positionAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_attachments.push_back(positionAttachment);

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = m_deviceRef->findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	m_attachments.push_back(depthAttachment);
    
    VkAttachmentReference albedoRef{};
    albedoRef.attachment = 0;
    albedoRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference normalRef{};
    normalRef.attachment = 1;
    normalRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference positionRef{};
    positionRef.attachment = 2;
    positionRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	m_colorAttachmentRefs.push_back(albedoRef);
	m_colorAttachmentRefs.push_back(normalRef);
	m_colorAttachmentRefs.push_back(positionRef);

    VkAttachmentReference depthRef{};
    depthRef.attachment = 3;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	m_depthAttachmentRef = depthRef;


    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 3;
    subpass.pColorAttachments = m_colorAttachmentRefs.data();
    subpass.pDepthStencilAttachment = &m_depthAttachmentRef;
	m_subpasses.push_back(subpass);

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	m_dependencies.push_back(dependency);


}

void lve::DeferredRenderer::getAttachementView(uint32_t p_imgIndex, std::vector<VkImageView>& p_outputAttachement)
{
    p_outputAttachement.clear();
	m_gBuffer->getImageViews(p_imgIndex, p_outputAttachement);
}

void lve::DeferredRenderer::onSwapChainRecreated()
{
    m_gBuffer = std::make_unique<GBuffer>(m_swapChain->getSwapChainExtent(), m_swapChain->imageCount());
}

void lve::DeferredRenderer::createGeometryRenderPass()
{

}



