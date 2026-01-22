#include "LightingPassDeferred.h"
#include "SceneManager.h"
#include <algorithm>

namespace lve {
 

    LightingPassDeferred::LightingPassDeferred(VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& p_descriptorSetLayout,std::shared_ptr<GBuffer> p_gBuffer)
        : RenderSystem(),
		m_gBuffer{ p_gBuffer }
    {

		p_descriptorSetLayout.push_back(SceneManager::getInstance()->getLightDescriptorSetLayout());
        uint32_t pushConstantSize = sizeof(SimplePushConstantData);
        init(renderPass, p_descriptorSetLayout, pushConstantSize, "shaders/LightningPassDeffered.vert.spv", "shaders/LightningPassDeffered.frag.spv");
        m_pushConstants._lightNumber = SceneManager::getInstance()->getLightManager().getLightCount();
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

        auto lightSet=SceneManager::getInstance()->getLightDescriptorSet()[frameInfo.frameIndex];
         vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            2, 1,
            &lightSet,
            0, nullptr
		 );
     
         vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,  VK_SHADER_STAGE_VERTEX_BIT| VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &m_pushConstants);



        // FULLSCREEN TRIANGLE
        vkCmdDraw(frameInfo.commandBuffer, 3, 1, 0, 0);
    }




    void LightingPassDeferred::drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs)
    {
    
    }

    void LightingPassDeferred::createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut)
    {
        LvePipeline::defaultFowardPipelineConfigInfo(p_pipelineInfoOut);
    }

}
