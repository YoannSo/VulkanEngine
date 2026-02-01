#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <cassert>

#include <Renderer/RenderSystems/RenderSystem.h>
#include <Vulkan/Device.hpp>
#include   <Renderer/Ressources/FrameInfo.hpp>
namespace engine::stages {
	class RenderStage
	{
	public:
		RenderStage(const engine::Device& p_device, const SwapChain& p_swapchain)
			:m_device(p_device), m_swapchain(const_cast<SwapChain&>(p_swapchain))
		{
		}
		void record(const FrameInfo& p_frameInfo, int p_currentSwapchainImage);
		void init();
	protected:
		virtual void createRenderPass() = 0;
		virtual void createFrameBuffers() = 0;

		virtual void beginRenderPass(const FrameInfo& p_frameInfo,int p_currentSwapchainImage) ;
		void endRenderPass(const FrameInfo& p_frameInfo);

		virtual std::vector<VkClearValue> getClearValues()=0;
	protected:
		VkRenderPass m_renderPass{ VK_NULL_HANDLE };
		std::vector<VkFramebuffer> m_frameBuffers;

		std::vector<std::unique_ptr<RenderSystem>> m_renderSystems;
		const Device& m_device;
		SwapChain& m_swapchain;


	};
}
