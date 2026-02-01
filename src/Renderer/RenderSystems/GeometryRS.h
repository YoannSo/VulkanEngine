#pragma once

#include <map>
#include <gtc/constants.hpp>
#include <glm.hpp>

#include <Renderer/RenderSystems/RenderSystem.h>

namespace engine {

	class GeometryPassRenderSystem : public RenderSystem {

	public:


		GeometryPassRenderSystem(VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& p_descriptorSetLayout);//globalSetLayout to tell the pipeline what descriptor set layout will be
		~GeometryPassRenderSystem();

		GeometryPassRenderSystem(const GeometryPassRenderSystem&) = delete; // delete copy constructor
		GeometryPassRenderSystem& operator=(const GeometryPassRenderSystem&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		void update(FrameInfo& frameInfo, SceneManager::GlobalUbo& ubo) override {}
		void render(const FrameInfo& frameInfo)override;// camera not meber bs we want to be able to share camera object multiple mtiple render system

	private:
		void drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs);
		void createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut) override;

	};
}