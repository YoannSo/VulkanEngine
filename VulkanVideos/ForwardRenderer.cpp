#include "ForwardRenderer.hpp"


lve::ForwardRenderer::ForwardRenderer(LveWindow& window, LveDevice* p_deviceRef) :LveRenderer(window, p_deviceRef)
{
    std::cout << "CREATION FORWARD RENDERER"<<std::endl;
    init();
}

lve::ForwardRenderer::~ForwardRenderer()
{
}

void lve::ForwardRenderer::fillRenderPassInfo()
{

    m_attachments.clear();
    m_subpasses.clear();
    m_dependencies.clear();
    m_colorAttachmentRefs.clear();

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = m_deviceRef->findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    m_depthAttachmentRef = depthAttachmentRef;
 
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_swapChain->getSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    m_attachments.push_back(colorAttachment);
    m_attachments.push_back(depthAttachment);

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	m_colorAttachmentRefs.push_back(colorAttachmentRef);

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = m_colorAttachmentRefs.data();
    subpass.pDepthStencilAttachment = &m_depthAttachmentRef;

	m_subpasses.push_back(subpass);

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	m_dependencies.push_back(dependency);


}

void lve::ForwardRenderer::getAttachementView(uint32_t p_imgIndex, std::vector<VkImageView>& p_outputAttachement)
{
    p_outputAttachement.clear();
    p_outputAttachement.push_back(m_swapChain->getImageView(p_imgIndex));
	p_outputAttachement.push_back(m_swapChain->getDepthImageView(p_imgIndex));
}

void lve::ForwardRenderer::createRenderSystems(VkDescriptorSetLayout p_globalDescriptorSet)
{
	m_renderSystems.push_back(std::make_unique<SimpleRenderSystem>(m_renderPass, p_globalDescriptorSet));
	m_renderSystems.push_back(std::make_unique<PointLighRenderSystem>(m_renderPass, p_globalDescriptorSet));
}


