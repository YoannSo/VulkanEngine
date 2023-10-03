#pragma once

#include "window.hpp"
#include "lve_game_object.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"
#include "lve_model.hpp"
#include "model.hpp"
#include "lve_descriptor.hpp"
#include <memory>
#include <vector>
namespace lve {

	class FirstApp {

	public:
		static constexpr int WIDTH = 800;//cst for w and h
		static constexpr int HEIGHT = 800;

		FirstApp();
		~FirstApp();
		
		FirstApp(const FirstApp&) = delete; // delete copy constructor
		FirstApp& operator=(const FirstApp&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		void init_imgui();
		void run();
	private:
		void loadGameObjects();
	//	void createPipelineLayout();
	//	void createPipeline();
	//	void renderGameObjects(VkCommandBuffer vkCOmmandBufferParameters);

		//order matter init top to bottom destroy bottom to top
		LveWindow _window{ WIDTH,HEIGHT,"FirstApp" };// first app created auto creat window destroy auto destroy
		LveDevice lveDevice{ _window };
		LveRenderer lveRenderer{ _window,lveDevice };
		std::unique_ptr<LveDescriptorPool> _globalPool{};
		//std::unique_ptr<LveTexture> texture{};
		LveGameObject::Map gameObjects;
	};
}