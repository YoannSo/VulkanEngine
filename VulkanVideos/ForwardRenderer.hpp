#pragma once

#include <array>
#include <iostream>
#include "lve_renderer.hpp"

#include "point_light_system.hpp"
#include "simple_render_system.hpp"
namespace lve {

	class ForwardRenderer:public LveRenderer {

	public:
		ForwardRenderer(LveWindow& window, LveDevice* p_deviceRef);
		~ForwardRenderer();

		virtual void createRenderSystems(VkDescriptorSetLayout p_globalDescriptorLayout)override;

	private:
		void fillRenderPassInfo() override;


	};
}