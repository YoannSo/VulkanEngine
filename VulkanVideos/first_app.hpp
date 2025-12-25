#pragma once


#include <memory>
#include <vector>

#include "SceneManager.h"
#include "window.hpp"
#include "GameObject.hpp"
#include "lve_device.hpp"
#include "ForwardRenderer.hpp"
#include "DeferredRenderer.hpp"
#include "lve_model.hpp"
#include "model.hpp"
#include "lve_descriptor.hpp"
#include "GuiManager.h"
namespace lve {

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
	//	void createPipelineLayout();
	//	void createPipeline();
	//	void renderGameObjects(VkCommandBuffer vkCOmmandBufferParameters);

		//order matter init top to bottom destroy bottom to top
		LveWindow _window{ WIDTH,HEIGHT,"FirstApp" };// first app created auto creat window destroy auto destroy
		DeferredRenderer lveRenderer{ _window ,LveDevice::getInstance() };
		std::unique_ptr<LveDescriptorPool> _globalPool{};
		std::unique_ptr<GuiManager> m_guiManager;

#ifndef NDEBUG
        // GPU timestamp query pool for profiling (only in debug builds)
        VkQueryPool m_queryPool{ VK_NULL_HANDLE };
        bool m_enableProfiling{ true };
        int m_frameLogCounter{ 0 };
#endif
		//std::unique_ptr<LveTexture> texture{};

		std::shared_ptr<SceneManager> m_sceneManager;
	};
}