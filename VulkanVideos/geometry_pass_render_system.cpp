#include "geometry_pass_render_system.h"
#include "SceneManager.h"
#include <algorithm>

namespace lve {
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f }; // offset homegous 
        glm::mat4 normalMatrix{ 1.f };
    };


    GeometryPassRenderSystem::GeometryPassRenderSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : RenderSystem()  // call base class constructor
    {
        auto layouts = buildLayouts(globalSetLayout);
        uint32_t pushConstantSize = sizeof(SimplePushConstantData);
        init(renderPass, layouts, pushConstantSize, "shaders/gBuffer.vert.spv", "shaders/gBuffer.frag.spv");

    }
    GeometryPassRenderSystem::~GeometryPassRenderSystem() {
    }

    void GeometryPassRenderSystem::render(FrameInfo& frameInfo)
    {
        lvePipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
            0, // starting set
            1, // number of sets
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );

        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
            1, // starting set
            1, // number of sets
            &(SceneManager::getInstance()->getGlobalDescriptorSet()[frameInfo.frameIndex]),
            0,
            nullptr
        );

        drawBatch(frameInfo, SceneManager::getInstance()->getOpaqueRenderingBatch());
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

    void GeometryPassRenderSystem::createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut)
    {
         LvePipeline::defaultDeferredPipelineConfigInfo(p_pipelineInfoOut);
    }

}
