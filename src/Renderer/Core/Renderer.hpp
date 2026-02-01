#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <array>

#include <Vulkan/Window.hpp>
#include <Vulkan/Device.hpp>
#include <Renderer/RenderSystems/RenderSystem.h>
#include <Engine/Managers/SceneManager.h>
#include <Renderer/Stages/RenderStage.h>

namespace engine {

	class Renderer {

	public:

		Renderer(Window& window, Device* p_deviceRef);
		~Renderer();
		//Renderer(const Renderer&) = delete; // delete copy constructor
		//Renderer& operator=(const Renderer&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed


		void init();
		VkCommandBuffer beginFrame(); //can't be private that use outside , split drawfram into 
		//part, one begn frame one ending frame
		void endFrame();

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in prigress");
			return commandBuffers[currentFrameIndex];
		}

		float getAspectRatio()const { return m_swapChain->extentAspectRatio(); }
		int getFrameIndex() const {
			assert(isFrameStarted && "connot get frame index not in progress ");
			return currentFrameIndex;
		};

		inline uint32_t getImageCount() const { return m_swapChain->imageCount(); }

		virtual void render(const FrameInfo& frameInfo);

		virtual void createRenderStages() = 0;

	private:
		void recreateSwapChain();
		void freeCommandBuffers();
		void createCommandBuffers();

		virtual void onSwapChainRecreated() {}


	protected:
		std::unique_ptr<SwapChain> m_swapChain;//using pointer small performance cost
		const Device* m_deviceRef;
		SceneManager* m_sceneManagerRef;

		uint32_t currentImgIndex;
		bool isFrameStarted{ false };
		int currentFrameIndex{ 0 };

		std::vector<std::unique_ptr<stages::RenderStage>> m_renderStages;


	private:

		Window& _window;// first app created auto creat window destroy auto destroy
		std::vector<VkCommandBuffer> commandBuffers;

	};
}
