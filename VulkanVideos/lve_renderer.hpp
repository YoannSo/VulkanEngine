#pragma once

#include <memory>
#include <vector>
#include <cassert>

#include "window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"
#include "g_buffer.hpp"
namespace lve {

	class LveRenderer {

	public:

		LveRenderer(LveWindow &window);
		~LveRenderer();
		//LveRenderer(const LveRenderer&) = delete; // delete copy constructor
		//LveRenderer& operator=(const LveRenderer&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed


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
				VkRenderPass getSwapChainRenderPass() const {
					return lveSwapChain->getRenderPass();
				}
				float getAspectRatio()const { return lveSwapChain->extentAspectRatio(); }
				int getFrameIndex() const {
					assert(isFrameStarted && "connot get frame index not in progress ");
					return currentFrameIndex;
				};

				inline uint32_t getImageCount() const { return lveSwapChain->imageCount(); }

		// helpers for GPU timestamp profiling - optional
		void writeTimestampStart(VkCommandBuffer cmd, VkQueryPool queryPool, uint32_t index);
		void writeTimestampEnd(VkCommandBuffer cmd, VkQueryPool queryPool, uint32_t index);

	private:
		void recreateSwapChain();
		void recreateGBuffer();

		void freeCommandBuffers();


		void createCommandBuffers();

		LveWindow& _window;// first app created auto creat window destroy auto destroy
		std::unique_ptr<LveSwapChain> lveSwapChain;//using pointer small performance cost
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImgIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };

		std::unique_ptr<GBuffer> m_gBuffer;
	};
}