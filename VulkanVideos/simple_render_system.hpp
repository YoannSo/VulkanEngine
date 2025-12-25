#pragma once


#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1

#include <map>
#include <gtc/constants.hpp>
#include <glm.hpp>

#include "lve_model.hpp"
#include "RenderSystem.h"
namespace lve {

	class SimpleRenderSystem : public RenderSystem{

	public:


		SimpleRenderSystem( VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);//globalSetLayout to tell the pipeline what descriptor set layout will be
		~SimpleRenderSystem();
		
		SimpleRenderSystem(const SimpleRenderSystem&) = delete; // delete copy constructor
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		void update(FrameInfo& frameInfo, GlobalUbo& ubo) override {}
		void render(FrameInfo& frameInfo)override;// camera not meber bs we want to be able to share camera object multiple mtiple render system

	private:
		void drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batch);//draw all game objects
		void drawTransparentBatch(FrameInfo& frameInfo, SceneManager::TransparentRenderingBatch& batch);//draw all game objects

	};
}