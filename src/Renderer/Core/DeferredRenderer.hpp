#pragma once

#include <array>
#include <memory>

#include <Renderer/Core/Renderer.hpp>
#include <Renderer/Ressources/GBuffer.hpp>
#include <Renderer/Stages/GeometryStage.h>
#include <Renderer/Stages/LightingStage.h>
#include <Vulkan/Window.hpp>
namespace engine {

	class DeferredRenderer :public Renderer {

	public:
		DeferredRenderer(Window& window, Device* p_deviceRef);
		~DeferredRenderer();

		void createRenderStages() override;

	private:
		void onSwapChainRecreated() override;


	private:
		std::shared_ptr<GBuffer> m_gBuffer;
	};
}