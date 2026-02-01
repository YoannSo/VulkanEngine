#pragma once

#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1

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
		static constexpr int WIDTH = 1400;//cst for w and h
		static constexpr int HEIGHT = 800;

		FirstApp();
		~FirstApp();
		
		FirstApp(const FirstApp&) = delete; // delete copy constructor
		FirstApp& operator=(const FirstApp&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		void run();
	private:
		void loadGameObjects();

		//order matter init top to bottom destroy bottom to top
		Window _window{ WIDTH,HEIGHT,"FirstApp" };// first app created auto creat window destroy auto destroy
		DeferredRenderer Renderer{ _window ,Device::getInstance() };
		std::unique_ptr<DescriptorPool> _globalPool{};
		std::unique_ptr<GuiManager> m_guiManager;

#ifndef NDEBUG
        // GPU timestamp query pool for profiling (only in debug builds)
        VkQueryPool m_queryPool{ VK_NULL_HANDLE };
        bool m_enableProfiling{ true };
        int m_frameLogCounter{ 0 };
#endif
		//std::unique_ptr<Texture> texture{};

		std::shared_ptr<SceneManager> m_sceneManager;
	};
}