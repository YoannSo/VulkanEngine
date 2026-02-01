#pragma once
#include <array>
#include <Renderer/Stages/RenderStage.h>
#include <Renderer/Ressources/GBuffer.hpp>
#include <Renderer/RenderSystems/LighningRS.h>
namespace engine::stages {

	class LightingStage :public RenderStage
	{
		public:
			LightingStage(const engine::Device& p_device, const SwapChain& m_swapchain, std::shared_ptr<GBuffer> p_gBuffer, const SceneManager& p_sceneManager);
	protected:
		void createRenderPass() override;
			void createFrameBuffers() override;
			std::vector<VkClearValue> getClearValues() override;
	private:
		std::shared_ptr<GBuffer> m_gBuffer;
	};
}
