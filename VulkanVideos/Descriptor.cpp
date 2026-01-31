#include "Descriptor.hpp"

// std
#include <cassert>
#include <stdexcept>
#include <iostream>

namespace engine {

    // *************** Descriptor Set Layout Builder *********************

    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding( //check iof binding dont already exist at this space, configure vulkan sctruct with descriptor layout
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
        return std::make_unique<DescriptorSetLayout>(bindings);
    }

    // *************** Descriptor Set Layout *********************

    DescriptorSetLayout::DescriptorSetLayout( // take the map of binding, 
         std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : bindings{ bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{}; // create a vector with just setLayoutCinding,
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};//create info 
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;


        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());

        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(//create a set layout and store in class member variable
            Device::getInstance()->getDevice(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout() { //destroy when no longer needed 
        vkDestroyDescriptorSetLayout(Device::getInstance()->getDevice(), descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
        poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
        poolFlags = flags;
        return *this;
    }
    DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32_t count) {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
        return std::make_unique<DescriptorPool>( maxSets, poolFlags, poolSizes);
    }

    // *************** Descriptor Pool *********************

    DescriptorPool::DescriptorPool(
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{}; //create info struct with information
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(Device::getInstance()->getDevice(), &descriptorPoolInfo, nullptr, &descriptorPool) !=//then createfunction from vulkan
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

    }

    DescriptorPool::~DescriptorPool() {
        vkDestroyDescriptorPool(Device::getInstance()->getDevice(), descriptorPool, nullptr);//clean when dont needed
    }

    bool DescriptorPool::allocateDescriptor(// allocate single descriptor SET (not one descriptor) from the pool 
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;// pool knwo how many descriptoir and what trype
        allocInfo.descriptorSetCount = 1;



        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        auto result = vkAllocateDescriptorSets(Device::getInstance()->getDevice(), &allocInfo, &descriptor);
        if (result != VK_SUCCESS) {// need to have descriptor remaining in the pool to create one, to avoid that we can have a pool manageer 
            return false;
        }
        return true;
    }

    void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const { //free q specify list of descriptor or reset entire pool
        vkFreeDescriptorSets(
            Device::getInstance()->getDevice(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void DescriptorPool::resetPool() {
        vkResetDescriptorPool(Device::getInstance()->getDevice(), descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    DescriptorWriter& DescriptorWriter::writeBuffer( // take binding nu;ber, qnd pointer to bufferInfoObject 
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");//firsdt heck if binding nimber hqs be define in the binding description of the setLqyout

        auto& bindingDescription = setLayout.bindings[binding];

        VkWriteDescriptorSet write{}; // then create a VKObject to add in the descriptor array of write
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    DescriptorWriter& DescriptorWriter::writeImage(//take poiter to iamge info, 
        uint32_t binding, VkDescriptorImageInfo* imageInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }
    DescriptorWriter& DescriptorWriter::writeImages(//take poiter to iamge info, 
        uint32_t binding, std::vector<VkDescriptorImageInfo>& imageInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];
        assert(
            bindingDescription.descriptorCount > 0 &&
            "Binding single descriptor info, but binding expects multiple ");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.dstArrayElement = 0; // Start at the first element
        write.pImageInfo = imageInfo.data();
        write.descriptorCount = static_cast<uint32_t>(imageInfo.size());

        writes.push_back(write);
        return *this;
    }

    bool DescriptorWriter::build(VkDescriptorSet& set) {//allocate descriptor set from the provider descriptor pool

        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void DescriptorWriter::overwrite(VkDescriptorSet& set) {//set target dst of all descriptor to write in the next descriptor set 
        for (auto& write : writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(Device::getInstance()->getDevice(), writes.size(), writes.data(), 0, nullptr);
    }

}  // namespace engine