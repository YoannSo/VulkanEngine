#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "lve_device.hpp"
#include "lve_descriptor.hpp"
#include "lve_buffer.hpp"
#include "SceneManager.h"
namespace lve {
class GBuffer {
public:
    GBuffer(VkExtent2D extent, uint32_t imageCount);
    ~GBuffer();

    // create the framebuffers for the provided render pass (must match attachments)
    void createFramebuffers(VkRenderPass renderPass);

    // descriptor infos for lighting pass (albedo, normal, position)
    std::vector<VkDescriptorImageInfo> getDescriptorImageInfos(uint32_t index) const;

    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }
    VkExtent2D getExtent() const { return extent; }

    void getImageViews(uint32_t index, std::vector<VkImageView>& outputViews) const;

    VkDescriptorSetLayout getDescritporSetLayout() const {
        return m_descriptorSetLayout->getDescriptorSetLayout();
	}
	const std::vector<VkDescriptorSet>& getDescriptorSets() const { return m_descriptorSet; }

private:
    void createImages();
    void createSampler();
    void createImageViews();
    void cleanup();
	void createDescriptorSetLayout();
	void createDescriptorSets();
private:

	std::vector<VkDescriptorSet> m_descriptorSet;
    std::unique_ptr<LveDescriptorSetLayout> m_descriptorSetLayout;
    std::vector<LveBuffer*> m_uboBuffer;

    LveDevice* device{ nullptr };
    VkExtent2D extent{};
    uint32_t imageCount{ 0 };

    // per-image arrays
    std::vector<VkImage> albedoImages;
    std::vector<VkDeviceMemory> albedoMem;
    std::vector<VkImageView> albedoViews;

    std::vector<VkImage> normalImages;
    std::vector<VkDeviceMemory> normalMem;
    std::vector<VkImageView> normalViews;

    std::vector<VkImage> positionImages;
    std::vector<VkDeviceMemory> positionMem;
    std::vector<VkImageView> positionViews;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthMem;
    std::vector<VkImageView> depthViews;

    VkSampler colorSampler{ VK_NULL_HANDLE };

    std::vector<VkFramebuffer> framebuffers;
};
}
