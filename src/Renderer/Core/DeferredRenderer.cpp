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
    m_renderStages.emplace_back(std::make_unique<stages::LightingStage>(*m_deviceRef, *m_swapChain, m_gBuffer, shadowMap, *m_sceneManagerRef ));
}

