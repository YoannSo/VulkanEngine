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
    m_renderStages.emplace_back(std::make_unique<stages::GeometryStage>(*m_deviceRef, *m_swapChain, m_gBuffer, *m_sceneManagerRef));
	m_renderStages.emplace_back(std::make_unique<stages::LightingStage>(*m_deviceRef, *m_swapChain, m_gBuffer, *m_sceneManagerRef));
}

