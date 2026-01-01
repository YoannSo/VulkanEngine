#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include <iostream>

#include "window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"
#include "g_buffer.hpp"
#include "RenderSystem.h"
namespace lve {

	class LveRenderer {

	public:

		LveRenderer(LveWindow& window, LveDevice* p_deviceRef);
		~LveRenderer();
		//LveRenderer(const LveRenderer&) = delete; // delete copy constructor
		//LveRenderer& operator=(const LveRenderer&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed


		void init();
		VkCommandBuffer beginFrame(); //can't be private that use outside , split drawfram into 
		//part, one begn frame one ending frame
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in prigress");
			return commandBuffers[currentFrameIndex];
		}
		//app will need to acces the swapchain render pass, to configue any pipeline
		VkRenderPass getRenderPass() const {
			return m_renderPass;
		}
		float getAspectRatio()const { return m_swapChain->extentAspectRatio(); }
		int getFrameIndex() const {
			assert(isFrameStarted && "connot get frame index not in progress ");
			return currentFrameIndex;
		};

		inline uint32_t getImageCount() const { return m_swapChain->imageCount(); }

		// helpers for GPU timestamp profiling - optional
		void writeTimestampStart(VkCommandBuffer cmd, VkQueryPool queryPool, uint32_t index);
		void writeTimestampEnd(VkCommandBuffer cmd, VkQueryPool queryPool, uint32_t index);

		virtual void createRenderSystems(VkDescriptorSetLayout p_globalDescriptorSet) = 0;
		void updateRenderSystems(FrameInfo& frameInfo, GlobalUbo& ubo);
		void renderRenderSystems(FrameInfo& frameInfo);

		virtual void render(FrameInfo& frameInfo);



	private:
		void recreateSwapChain();
		void recreateGBuffer();
		void freeCommandBuffers();
		void createCommandBuffers();
		void createFramebuffers();

		virtual void fillRenderPassInfo() = 0;
		virtual void getAttachementView(uint32_t p_imgIndex, std::vector<VkImageView>& p_outputAttachement) = 0;
		virtual void onSwapChainRecreated() {}

		void createRenderPass();


	protected:
		std::unique_ptr<LveSwapChain> m_swapChain;//using pointer small performance cost
		const LveDevice* m_deviceRef;

		std::vector<VkAttachmentDescription> m_attachments;
		std::vector<VkSubpassDescription> m_subpasses;
		std::vector<VkSubpassDependency> m_dependencies;

		std::vector<VkAttachmentReference> m_colorAttachmentRefs;
		VkAttachmentReference m_depthAttachmentRef;

		std::vector<std::unique_ptr<RenderSystem>> m_renderSystems;

		VkRenderPass m_renderPass;

		uint32_t currentImgIndex;
		bool isFrameStarted{ false };
		int currentFrameIndex{ 0 };


	private:


		LveWindow& _window;// first app created auto creat window destroy auto destroy
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkFramebuffer> m_frameBuffers;




	};
}