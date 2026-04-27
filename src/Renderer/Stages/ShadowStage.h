#pragma once
#include <array>
#include <Renderer/Stages/RenderStage.h>
#include <Renderer/Ressources/GBuffer.hpp>
#include <Engine/Managers/SceneManager.h>

#include <Renderer/RenderSystems/ShadowRS.h>
namespace engine::stages {

	
	class ShadowStage :public RenderStage
	{
	public:
		ShadowStage(const engine::Device& p_device, const SwapChain& m_swapchain,const SceneManager& p_sceneManager);
	protected:
			void createRenderPass() override;
			void createFrameBuffers() override;
			std::vector<VkClearValue> getClearValues() override;
	private:
		std::shared_ptr<Texture> m_shadowMap;
	};
}

