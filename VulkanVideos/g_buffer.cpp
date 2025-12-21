#include "g_buffer.hpp"
#include <stdexcept>
#include <array>
#include "lve_device.hpp"

namespace lve {

static VkFormat chooseColorFormat() {
    return VK_FORMAT_R8G8B8A8_UNORM;
}

static VkFormat chooseNormalFormat() {
    return VK_FORMAT_R16G16B16A16_SFLOAT;
}

static VkFormat choosePositionFormat() {
    return VK_FORMAT_R32G32B32A32_SFLOAT;
}

GBuffer::GBuffer(VkExtent2D extent, uint32_t imageCount)
    : device(LveDevice::getInstance()), extent(extent), imageCount(imageCount) {
    createImages();
    createImageViews();
    createSampler();
}

GBuffer::~GBuffer() {
    cleanup();
}

void GBuffer::createImages() {
    albedoImages.resize(imageCount);
    albedoMem.resize(imageCount);
    albedoViews.resize(imageCount);

    normalImages.resize(imageCount);
    normalMem.resize(imageCount);
    normalViews.resize(imageCount);

    positionImages.resize(imageCount);
    positionMem.resize(imageCount);
    positionViews.resize(imageCount);

    depthImages.resize(imageCount);
    depthMem.resize(imageCount);
    depthViews.resize(imageCount);

    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = extent.width;
        imageInfo.extent.height = extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        // albedo
        imageInfo.format = chooseColorFormat();
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        device->createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, albedoImages[i], albedoMem[i]);

        // normal
        imageInfo.format = chooseNormalFormat();
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        device->createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, normalImages[i], normalMem[i]);

        // position
        imageInfo.format = choosePositionFormat();
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        device->createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, positionImages[i], positionMem[i]);

        // depth
        imageInfo.format = device->findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        device->createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthMem[i]);
    }
}

void GBuffer::createImageViews() {
    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        viewInfo.image = albedoImages[i];
        viewInfo.format = chooseColorFormat();
        if (vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &albedoViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create albedo image view");
        }

        viewInfo.image = normalImages[i];
        viewInfo.format = chooseNormalFormat();
        if (vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &normalViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create normal image view");
        }

        viewInfo.image = positionImages[i];
        viewInfo.format = choosePositionFormat();
        if (vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &positionViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create position image view");
        }

        VkImageViewCreateInfo depthViewInfo = {};
        depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewInfo.image = depthImages[i];
        depthViewInfo.format = device->findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthViewInfo.subresourceRange.baseMipLevel = 0;
        depthViewInfo.subresourceRange.levelCount = 1;
        depthViewInfo.subresourceRange.baseArrayLayer = 0;
        depthViewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device->getDevice(), &depthViewInfo, nullptr, &depthViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create depth image view");
        }
    }
}

void GBuffer::createSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &colorSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create sampler for gbuffer");
    }
}

void GBuffer::createFramebuffers(VkRenderPass renderPass) {
    framebuffers.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        std::array<VkImageView, 4> attachments = { albedoViews[i], normalViews[i], positionViews[i], depthViews[i] };

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = renderPass;
        fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = extent.width;
        fbInfo.height = extent.height;
        fbInfo.layers = 1;

        if (vkCreateFramebuffer(device->getDevice(), &fbInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create gbuffer framebuffer");
        }
    }
}

std::vector<VkDescriptorImageInfo> GBuffer::getDescriptorImageInfos(uint32_t index) const {
    std::vector<VkDescriptorImageInfo> infos;
    VkDescriptorImageInfo albedoInfo{};
    albedoInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    albedoInfo.imageView = albedoViews[index];
    albedoInfo.sampler = colorSampler;
    infos.push_back(albedoInfo);

    VkDescriptorImageInfo normalInfo{};
    normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    normalInfo.imageView = normalViews[index];
    normalInfo.sampler = colorSampler;
    infos.push_back(normalInfo);

    VkDescriptorImageInfo posInfo{};
    posInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    posInfo.imageView = positionViews[index];
    posInfo.sampler = colorSampler;
    infos.push_back(posInfo);

    return infos;
}

void GBuffer::cleanup() {
    if (colorSampler != VK_NULL_HANDLE) vkDestroySampler(device->getDevice(), colorSampler, nullptr);

    for (auto fb : framebuffers) vkDestroyFramebuffer(device->getDevice(), fb, nullptr);

    for (auto v : albedoViews) vkDestroyImageView(device->getDevice(), v, nullptr);
    for (auto v : normalViews) vkDestroyImageView(device->getDevice(), v, nullptr);
    for (auto v : positionViews) vkDestroyImageView(device->getDevice(), v, nullptr);
    for (auto v : depthViews) vkDestroyImageView(device->getDevice(), v, nullptr);

    for (auto img : albedoImages) vkDestroyImage(device->getDevice(), img, nullptr);
    for (auto mem : albedoMem) vkFreeMemory(device->getDevice(), mem, nullptr);

    for (auto img : normalImages) vkDestroyImage(device->getDevice(), img, nullptr);
    for (auto mem : normalMem) vkFreeMemory(device->getDevice(), mem, nullptr);

    for (auto img : positionImages) vkDestroyImage(device->getDevice(), img, nullptr);
    for (auto mem : positionMem) vkFreeMemory(device->getDevice(), mem, nullptr);

    for (auto img : depthImages) vkDestroyImage(device->getDevice(), img, nullptr);
    for (auto mem : depthMem) vkFreeMemory(device->getDevice(), mem, nullptr);
}

}
