#include "LightingPassDeferred.h"
#include "SceneManager.h"
#include <algorithm>

namespace lve {
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f }; // offset homegous 
        glm::mat4 normalMatrix{ 1.f };
    };


    LightingPassDeferred::LightingPassDeferred(VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& p_descriptorSetLayout,std::shared_ptr<GBuffer> p_gBuffer)
        : RenderSystem(),
		m_gBuffer{ p_gBuffer }
    {

        uint32_t pushConstantSize = sizeof(SimplePushConstantData);
        init(renderPass, p_descriptorSetLayout, pushConstantSize, "shaders/LightningPassDeffered.vert.spv", "shaders/LightningPassDeffered.frag.spv");

    }
    LightingPassDeferred::~LightingPassDeferred() {
    }

    void LightingPassDeferred::render(FrameInfo& frameInfo)
    {
        lvePipeline->bind(frameInfo.commandBuffer);

        // Descriptor set global (camera + lights)
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0, 1,
            &frameInfo.globalDescriptorSet,
            0, nullptr
        );

        // Descriptor set GBuffer
        VkDescriptorSet gbufferSet = m_gBuffer->getDescriptorSets()[frameInfo.frameIndex];
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            1, 1,
            &gbufferSet,
            0, nullptr
        );

        // FULLSCREEN TRIANGLE
        vkCmdDraw(frameInfo.commandBuffer, 3, 1, 0, 0);
    }




    void LightingPassDeferred::drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs)
    {
        for (auto& batch : batchs) {
            auto material = SceneManager::getInstance()->getMaterialMap().find(batch.first);

            if (batch.second.size() > 0) {

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

    void LightingPassDeferred::createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut)
    {
        LvePipeline::defaultFowardPipelineConfigInfo(p_pipelineInfoOut);
    }

}
