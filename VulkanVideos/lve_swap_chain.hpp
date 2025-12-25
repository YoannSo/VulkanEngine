#pragma once

#include "lve_device.hpp"
// vulkan headers
#include <vulkan/vulkan.h>
// std lib headers
#include <string>
#include <vector>
#include <memory>
namespace lve {

class LveSwapChain {
 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
  enum class RenderPassMode { FORWARD, DEFERRED };

  LveSwapChain( VkExtent2D windowExtent, RenderPassMode mode = RenderPassMode::DEFERRED);
  LveSwapChain( VkExtent2D windowExtent,std::shared_ptr<LveSwapChain> previous, RenderPassMode mode = RenderPassMode::DEFERRED);
  ~LveSwapChain();

  LveSwapChain(const LveSwapChain &) = delete;
  LveSwapChain& operator=(const LveSwapChain &) = delete;

  VkImageView getImageView(int index) { return swapChainImageViews[index]; }
  VkImageView getDepthImageView(int index) { return depthImageViews[index]; }
  size_t imageCount() { return swapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return swapChainExtent; }
  uint32_t width() { return swapChainExtent.width; }
  uint32_t height() { return swapChainExtent.height; }

  float extentAspectRatio() {
    return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
  }
  VkResult acquireNextImage(uint32_t *imageIndex);
  VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
  bool compareSwapFormats(const LveSwapChain& swapChain)const {
      return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat;
  }

  RenderPassMode renderPassMode{ RenderPassMode::DEFERRED };

 private:
     void init();
  void createSwapChain();
  void createImageViews();
  void createDepthResources();
  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat swapChainImageFormat;
  VkFormat swapChainDepthFormat;
  VkExtent2D swapChainExtent;


  std::shared_ptr <LveSwapChain > oldSwapChain;
  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;
  size_t currentFrame = 0;

  LveDevice* m_deviceRef;
};

}  // namespace lve
