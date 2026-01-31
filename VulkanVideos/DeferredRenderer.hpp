#pragma once

#include <array>
#include <memory>

#include "Renderer.hpp"
#include "GBuffer.hpp"
#include "GeometryStage.h"
#include "LightingStage.h"
#include "Window.hpp"
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