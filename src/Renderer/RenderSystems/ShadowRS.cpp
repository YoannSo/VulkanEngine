#include "ShadowRS.h"
#include <Engine/Managers/SceneManager.h>
#include <algorithm>

namespace engine {
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f }; // offset homegous 
        glm::mat4 normalMatrix{ 1.f };
        uint32_t idMaterial=0;//x:idMaterial
    };


    ShadowRenderSystem::ShadowRenderSystem(VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& p_descriptorSetLayout)
        : RenderSystem()  // call base class constructor
    {
        uint32_t pushConstantSize = sizeof(SimplePushConstantData);
        init(renderPass, p_descriptorSetLayout, pushConstantSize, "shaders/shadow.vert.spv", "shaders/shadow.frag.spv");

    }
    ShadowRenderSystem::~ShadowRenderSystem() {
    }

    void ShadowRenderSystem::render(const FrameInfo& frameInfo)
    {
        m_pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
            0, // starting set
            1, // number of sets
            &(SceneManager::getInstance()->getGlobalDescriptorSets()[frameInfo.frameIndex]),
            0,
            nullptr
        );


        for(auto& object:SceneManager::getInstance()->getObjectMap()){
			Model* model = dynamic_cast<Model*>(object.second);

            for (auto& mesh : model->getMeshes()) {

                SimplePushConstantData push{};


                push.modelMatrix = object.second->transform.mat4();
                push.normalMatrix = object.second->transform.normalMatrix();

				push.idMaterial = mesh.getMaterialId();


                vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
                mesh.bind(frameInfo.commandBuffer, frameInfo.frameIndex, pipelineLayout);
                mesh.draw(frameInfo.commandBuffer);
            
            }
           

		}

        //drawBatch(frameInfo, SceneManager::getInstance()->getOpaqueRenderingBatch());
    }


 

    void ShadowRenderSystem::drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs)
    {

        for (auto& batch : batchs) {
            auto material = SceneManager::getInstance()->getMaterialMap().find(batch.first);

            if (batch.second.size() > 0) {
               /* vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                    2, // starting set
                    1, // number of sets
                    &(material->second->getDescriptorSet()[frameInfo.frameIndex]),
                    0,
                    nullptr
                );*/

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

    void ShadowRenderSystem::createPipelineInfo(PipelineConfigInfo& p_pipelineInfoOut)
    {
         Pipeline::shadowPipelineConfigInfo(p_pipelineInfoOut);
    }

}
