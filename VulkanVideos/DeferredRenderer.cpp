#include "DeferredRenderer.hpp"

lve::DeferredRenderer::DeferredRenderer(LveWindow& window, LveDevice* p_deviceRef) :LveRenderer(window, p_deviceRef)
{
    init();
}

lve::DeferredRenderer::~DeferredRenderer()
{
}

void lve::DeferredRenderer::createRenderSystems(VkDescriptorSetLayout p_globalDescriptorLayout)
{
    std::vector<VkDescriptorSetLayout> descriptorLayoutGeometryPass = { p_globalDescriptorLayout };
    std::vector<VkDescriptorSetLayout> descriptorLayoutLightningPass = { p_globalDescriptorLayout };
   
    std::vector<VkDescriptorSetLayout> materialLayouts =
		SceneManager::getInstance()->getMaterialSystemDescriptorSetLayouts();

    descriptorLayoutGeometryPass.insert(descriptorLayoutGeometryPass.end(),
        materialLayouts.begin(),
        materialLayouts.end());

    descriptorLayoutLightningPass.emplace_back(m_gBuffer->getDescritporSetLayout());

	m_renderSystems.push_back(std::make_unique<GeometryPassRenderSystem>(m_geometryRenderPass, descriptorLayoutGeometryPass));
	m_renderSystems.push_back(std::make_unique<LightingPassDeferred>(m_renderPass, descriptorLayoutLightningPass,m_gBuffer));
}

void lve::DeferredRenderer::fillRenderPassInfo()
{

  
}



void lve::DeferredRenderer::getAttachementView(uint32_t p_imgIndex, std::vector<VkImageView>& p_outputAttachement)
{
   // p_outputAttachement.clear();
   // m_gBuffer->getImageViews(p_imgIndex, p_outputAttachement);
}

void lve::DeferredRenderer::onSwapChainRecreated()
{
    m_gBuffer = std::make_shared<GBuffer>(m_swapChain->getSwapChainExtent(), m_swapChain->imageCount());
	createGeometryRenderPass();
	createGeometryFramebuffers();
}

void lve::DeferredRenderer::createGeometryRenderPass()
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
    depthAttachment.format = m_deviceRef->findDepthFormat();
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


    if (vkCreateRenderPass(m_deviceRef->getDevice(), &renderPassInfo, nullptr, &m_geometryRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create geometry render pass!");
    }
}

void lve::DeferredRenderer::createGeometryFramebuffers()
{
    m_geometryFrameBuffers.resize(m_swapChain->imageCount());
    for (size_t i = 0; i < m_swapChain->imageCount(); ++i) {

        std::vector<VkImageView> attachementView;
        m_gBuffer->getImageViews(i, attachementView);
;
        VkExtent2D swapChainExtent = m_swapChain->getSwapChainExtent();
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_geometryRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachementView.size());
        framebufferInfo.pAttachments = attachementView.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
            m_deviceRef->getDevice(),
            &framebufferInfo,
            nullptr,
            &m_geometryFrameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void lve::DeferredRenderer::beginGeometryRenderPass(VkCommandBuffer commandBuffer)
{
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");
    VkRenderPassBeginInfo renderPassInfo{}; //first command begin render pass
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_geometryRenderPass; // set render pass member swapchain as for us
    renderPassInfo.framebuffer = m_geometryFrameBuffers[currentImgIndex]; // which frame buffer this frame buffer ? 

    renderPassInfo.renderArea.offset = { 0,0 }; // render area, 
    renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();//swapchain extent instead of window extent

    std::array<VkClearValue, 4> clearValues{};//what we want the initiale value of frame buffer attachement

    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // albedo
    clearValues[1].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // normal
    clearValues[2].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // position
    clearValues[3].depthStencil = { 1.0f, 0 };                 // depth



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

void lve::DeferredRenderer::endGeometryRenderPass(VkCommandBuffer commandBuffer)
{
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
}

void lve::DeferredRenderer::render(FrameInfo& frameInfo)
{

	beginGeometryRenderPass(frameInfo.commandBuffer);
    m_renderSystems[0]->render(frameInfo);
	endGeometryRenderPass(frameInfo.commandBuffer);

	beginSwapChainRenderPass(frameInfo.commandBuffer);
	m_renderSystems[1]->render(frameInfo);
	endSwapChainRenderPass(frameInfo.commandBuffer);

	//LveRenderer::render(frameInfo);
}



