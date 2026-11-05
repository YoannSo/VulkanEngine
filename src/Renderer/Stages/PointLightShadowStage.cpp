#include "PointLightShadowStage.h"

namespace engine::stages
{

    PointLightShadowStage::PointLightShadowStage(const engine::Device& p_device, const SwapChain& p_swapchain, const SceneManager& p_sceneManager)
        : RenderStage(p_device, p_swapchain)
    {
        m_shadowCubemap = std::make_shared<Texture>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, VK_FORMAT_R32_SFLOAT,
                                                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, false, 1, false,
                                                    true // isDepth=false, mipLevels=1, compareEnable=false, isCubemap=true
        );

        m_shadowDepth = std::make_shared<Texture>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, VK_FORMAT_D32_SFLOAT,
                                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, true, 1, false, false // isDepth=true, isCubemap=false
        );

        init(); // createRenderPass() + createFrameBuffers()

      //  m_renderSystems.emplace_back(std::make_unique<PointLightShadowRS>(m_renderPass, SceneManager::getInstance()->getPool()));
    }

    void PointLightShadowStage::createRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_R32_SFLOAT;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // jamais lue après coup
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

        VkAttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthRef{};
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = &depthRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create point light shadow render pass!");
        }
    }

    void PointLightShadowStage::createFrameBuffers()
    {
        for (uint32_t face = 0; face < 6; ++face)
        {
            std::array<VkImageView, 2> attachments = {m_shadowCubemap->getCubemapImageView(face), m_shadowDepth->getImageView()};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = SHADOW_MAP_RESOLUTION;
            framebufferInfo.height = SHADOW_MAP_RESOLUTION;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_device.getDevice(), &framebufferInfo, nullptr, &m_faceFrameBuffers[face]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create point light shadow framebuffer!");
            }
        }
    }

    std::vector<VkClearValue> PointLightShadowStage::getClearValues()
    {
        std::vector<VkClearValue> clearValues(2);
        clearValues[0].color = {{SHADOW_FAR, 0.f, 0.f, 0.f}}; // distance "infinie" par défaut
        clearValues[1].depthStencil = {1.0f, 0};
        return clearValues;
    }

    glm::mat4 PointLightShadowStage::getFaceProjectionMatrix() const
    {
        return glm::perspective(glm::radians(90.0f), 1.0f, SHADOW_NEAR, SHADOW_FAR);
    }

    std::array<glm::mat4, 6> PointLightShadowStage::getFaceViewMatrices() const
    {
        return {
            glm::lookAt(m_lightPosition, m_lightPosition + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),  // +X
            glm::lookAt(m_lightPosition, m_lightPosition + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)), // -X
            glm::lookAt(m_lightPosition, m_lightPosition + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),   // +Y
            glm::lookAt(m_lightPosition, m_lightPosition + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)), // -Y
            glm::lookAt(m_lightPosition, m_lightPosition + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),  // +Z
            glm::lookAt(m_lightPosition, m_lightPosition + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))  // -Z
        };
    }

    void PointLightShadowStage::record(const FrameInfo& p_frameInfo, int /*p_currentSwapchainImage*/)
    {
        /* auto* rs = static_cast<PointLightShadowRS*>(m_renderSystems[0].get());
        glm::mat4 projection = getFaceProjectionMatrix();
        auto viewMatrices = getFaceViewMatrices();
        auto clearValues = getClearValues();

        for (uint32_t face = 0; face < 6; ++face)
        {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_renderPass;
            renderPassInfo.framebuffer = m_faceFrameBuffers[face];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = {SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(p_frameInfo.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(SHADOW_MAP_RESOLUTION);
            viewport.height = static_cast<float>(SHADOW_MAP_RESOLUTION);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, {SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION}};
            vkCmdSetViewport(p_frameInfo.commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(p_frameInfo.commandBuffer, 0, 1, &scissor);

            glm::mat4 viewProj = projection * viewMatrices[face];
            rs->renderFace(p_frameInfo.commandBuffer, p_frameInfo.frameIndex, viewProj, m_lightPosition, SHADOW_FAR);

            vkCmdEndRenderPass(p_frameInfo.commandBuffer);
        }*/
    }
} // namespace engine::stages