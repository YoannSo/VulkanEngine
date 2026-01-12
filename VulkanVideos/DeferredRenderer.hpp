#pragma once

#include <array>
#include <memory>

#include "lve_renderer.hpp"
#include "g_buffer.hpp"
#include "geometry_pass_render_system.h"
#include "LightingPassDeferred.h"
namespace lve {

	class DeferredRenderer :public LveRenderer {

	public:
		DeferredRenderer(LveWindow& window, LveDevice* p_deviceRef);
		~DeferredRenderer();

		virtual void createRenderSystems(std::vector<VkDescriptorSetLayout>& p_globalDescriptorSet)override;
		void render(FrameInfo& frameInfo) override;

	private:
		void fillRenderPassInfo() override;
		void getAttachementView(uint32_t p_imgIndex, std::vector<VkImageView>& p_outputAttachement)override;
		void onSwapChainRecreated() override;


		void createGeometryRenderPass();
		void createGeometryFramebuffers();

		void beginGeometryRenderPass(VkCommandBuffer commandBuffer);
		void endGeometryRenderPass(VkCommandBuffer commandBuffer);

	private:
		std::shared_ptr<GBuffer> m_gBuffer;
		VkRenderPass m_geometryRenderPass;
		std::vector<VkFramebuffer> m_geometryFrameBuffers;
	};
}