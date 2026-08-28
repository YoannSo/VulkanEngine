#include <Renderer/Core/DeferredRenderer.hpp>

engine::DeferredRenderer::DeferredRenderer(Window& window, Device* p_deviceRef) :Renderer(window, p_deviceRef)
{
    init();
}

engine::DeferredRenderer::~DeferredRenderer()
{
}

void engine::DeferredRenderer::onSwapChainRecreated()
{
    m_gBuffer = std::make_shared<GBuffer>(m_swapChain->getSwapChainExtent(), m_swapChain->imageCount());
}

void engine::DeferredRenderer::createRenderStages()
{
    auto shadowStage = std::make_unique<stages::ShadowStage>(*m_deviceRef, *m_swapChain, *m_sceneManagerRef);
    std::shared_ptr<Texture> shadowMap = shadowStage->getShadowMap();
    m_renderStages.emplace_back(std::move(shadowStage));

    m_renderStages.emplace_back(std::make_unique<stages::GeometryStage>(*m_deviceRef, *m_swapChain, m_gBuffer, *m_sceneManagerRef));

    auto lightingStage = std::make_unique<stages::LightingStage>(*m_deviceRef, *m_swapChain, m_gBuffer, shadowMap, *m_sceneManagerRef);
    m_lightingStage = lightingStage.get();
    m_renderStages.emplace_back(std::move(lightingStage));
}
VkRenderPass engine::DeferredRenderer::getUIRenderPass() const
{
    return m_lightingStage ? m_lightingStage->getRenderPass() : VK_NULL_HANDLE;
}

void engine::DeferredRenderer::setUIRenderCallback(std::function<void(VkCommandBuffer)> p_callback)
{
    if (m_lightingStage)
    {
        m_lightingStage->setUIRenderCallback(p_callback);
    }
}