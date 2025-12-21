#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h" 
#include "imgui_impl_vulkan.h"

#define LVF_USE_IMGUI 1




struct GLFWwindow;
class GuiManager {
 public:
  GuiManager() = default;
  ~GuiManager();

  // Initialize the GUI system (no-op if ImGui not enabled). You can pass
  // required Vulkan handles here when you integrate ImGui.
  void init(GLFWwindow* window,
            VkInstance instance,
            VkDevice device,
            VkPhysicalDevice physicalDevice,
            uint32_t graphicsQueueFamily,
            VkQueue graphicsQueue,
            VkRenderPass renderPass,
            VkDescriptorPool descriptorPool,
            uint32_t minImageCount);

  // Call at the beginning of each frame (before recording UI draw calls)
  void newFrame();

  // Record GUI draw into the given command buffer. If ImGui is not used,
  // this is a no-op.
  void render(VkCommandBuffer commandBuffer);

  // Clean up any resources. Safe to call even if not initialized.
  void cleanup();

  // If the render pass changes (swapchain recreate), notify the GUI manager.
  void recreate(VkRenderPass renderPass);

private:
	void setupTopMenuBar();

 private:
  bool m_initialized{false};
  GLFWwindow* m_window{nullptr};
  VkInstance m_instance{VK_NULL_HANDLE};
  VkDevice m_device{VK_NULL_HANDLE};
  VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};
  VkQueue m_graphicsQueue{VK_NULL_HANDLE};
  uint32_t m_graphicsQueueFamily{0};
  VkRenderPass m_renderPass{VK_NULL_HANDLE};
  VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
  uint32_t m_minImageCount{0};
  VkDescriptorPool m_imguiPool{VK_NULL_HANDLE};
};

