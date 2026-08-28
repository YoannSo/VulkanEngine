#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <memory>
#include <vector>
#include <iostream>
#include <glm.hpp>
#include <chrono>

#include <gtc/constants.hpp>
#include <Engine/Managers/SceneManager.h>

#include <Vulkan/Window.hpp>

#include <Engine/Managers/GuiManager.h>
#include <Engine/Objects/Camera.hpp>

#include <Renderer/Core/DeferredRenderer.hpp>


#include <Engine/Controls/KeyboardController.hpp>
#include <Renderer/Ressources/FrameInfo.hpp>
#include <Engine/Objects/GameObject.hpp>

namespace engine {

	class FirstApp {

	public:
		static constexpr int WIDTH = 1400;
		static constexpr int HEIGHT = 800;

		FirstApp();
		~FirstApp();
		
		FirstApp(const FirstApp&) = delete; // delete copy constructor
		FirstApp& operator=(const FirstApp&) = delete;  // delete copy assignment

		void run();
	private:
		void loadGameObjects();

		//order matter init top to bottom destroy bottom to top
		Window _window{ WIDTH, HEIGHT, "FirstApp" };
		DeferredRenderer Renderer{ _window ,Device::getInstance() };
		std::unique_ptr<DescriptorPool> _globalPool{};
		std::unique_ptr<GuiManager> m_guiManager;

#ifndef NDEBUG
        // GPU timestamp query pool for profiling (only in debug builds)
        VkQueryPool m_queryPool{ VK_NULL_HANDLE };
        bool m_enableProfiling{ true };
        int m_frameLogCounter{ 0 };
#endif

		std::shared_ptr<SceneManager> m_sceneManager;
	};
}