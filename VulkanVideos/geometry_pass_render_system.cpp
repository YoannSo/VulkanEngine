#include "geometry_pass_render_system.h"
#include "SceneManager.h"
#include <algorithm>

namespace lve {
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f }; // offset homegous 
        glm::mat4 normalMatrix{ 1.f };
    };


    GeometryPassRenderSystem::GeometryPassRenderSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : RenderSystem(renderPass, buildLayouts(globalSetLayout), sizeof(SimplePushConstantData), "shaders/gBuffer.vert.spv", "shaders/gBuffer.frag.spv")  // call base class constructor
    {
    }
    GeometryPassRenderSystem::~GeometryPassRenderSystem() {
    }

    void GeometryPassRenderSystem::render(FrameInfo& frameInfo)
    {
    }


    std::vector<VkDescriptorSetLayout> GeometryPassRenderSystem::buildLayouts(VkDescriptorSetLayout globalSetLayout) {

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        if (SceneManager::getInstance()->getTextureMap().size() > 0) {
            descriptorSetLayouts.push_back(SceneManager::getInstance()->getDescriptorSetLayout().getDescriptorSetLayout());
        }

        descriptorSetLayouts.push_back(SceneManager::getInstance()->getMaterialDescriptorSetLayout().getDescriptorSetLayout());

        return descriptorSetLayouts;
    }

    void GeometryPassRenderSystem::drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs)
    {
        for (auto& batch : batchs) {
            auto material = SceneManager::getInstance()->getMaterialMap().find(batch.first);

            if (batch.second.size() > 0) {
                vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                    2, // starting set
                    1, // number of sets
                    &(material->second->getDescriptorSet()[frameInfo.frameIndex]),
                    0,
                    nullptr
                );

                for (auto& object : batch.second) {
                    SimplePushConstantData push{};
                    push.modelMatrix = object->getModel()->transform.mat4();
                    push.normalMatrix = object->getModel()->transform.normalMatrix();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
                    object->bind(frameInfo.commandBuffer, frameInfo.frameIndex, pipelineLayout);
                    object->draw(frameInfo.commandBuffer);
                }
            }
        }
    }

}
