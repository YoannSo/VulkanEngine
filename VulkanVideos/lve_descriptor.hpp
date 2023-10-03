#pragma once

#include "lve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lve {

    class LveDescriptorSetLayout {
    public:
        class Builder {//tell the pipeline what gonna expect 
        public:
            Builder(LveDevice& lveDevice) : lveDevice{ lveDevice } {}

            Builder& addBinding( //what type of descriptor to expect ? will shader stgae will have acces ? each biding can have a array of descriptor of that type, can indexed 
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<LveDescriptorSetLayout> build() const;//create instance of the DescripotrSetLayout

        private:
            LveDevice& lveDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};//cosntruct the ordererd map; 
        };

        LveDescriptorSetLayout(
            LveDevice& lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~LveDescriptorSetLayout();
        LveDescriptorSetLayout(const LveDescriptorSetLayout&) = delete;
        LveDescriptorSetLayout& operator=(const LveDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        LveDevice& lveDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class LveDescriptorWriter;
    };





    class LveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(LveDevice& lveDevice) : lveDevice{ lveDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);//how many descriptor of each type to execpt 
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);//behavior of the pool object
            Builder& setMaxSets(uint32_t count);//total number of descriptor set
            std::unique_ptr<LveDescriptorPool> build() const;

        private:
            LveDevice& lveDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        LveDescriptorPool(
            LveDevice& lveDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~LveDescriptorPool();
        LveDescriptorPool(const LveDescriptorPool&) = delete;
        LveDescriptorPool& operator=(const LveDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        LveDevice& lveDevice;
        VkDescriptorPool descriptorPool;

        friend class LveDescriptorWriter;
    };




    class LveDescriptorWriter {
    public:
        LveDescriptorWriter(LveDescriptorSetLayout& setLayout, LveDescriptorPool& pool);

        LveDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        LveDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        LveDescriptorSetLayout& setLayout;
        LveDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace lve