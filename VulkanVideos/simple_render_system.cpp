#include "simple_render_system.hpp"

namespace lve {
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f }; // offset homegous 
        glm::mat4 normalMatrix{ 1.f };
    };

    SimpleRenderSystem::SimpleRenderSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : RenderSystem ()
    {
        auto layouts = buildLayouts(globalSetLayout);
        uint32_t pushConstantSize = sizeof(SimplePushConstantData);
        init(renderPass, layouts, pushConstantSize, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv");

    }

    SimpleRenderSystem::~SimpleRenderSystem() {
    }

   

   /* void createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

     
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayout.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(LveDevice::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>("shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
    }*/

    void SimpleRenderSystem::drawBatch(FrameInfo& frameInfo, SceneManager::RenderingBatch& batchs)
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

    void SimpleRenderSystem::drawTransparentBatch(FrameInfo& frameInfo, SceneManager::TransparentRenderingBatch& batch)
    {
		SceneManager::TransparentRenderingBatch& transparentsObject = SceneManager::getInstance()->getTransparentRenderingBatch();

        std::sort(transparentsObject.begin(), transparentsObject.end(),
            [&frameInfo](const std::pair<std::string, TriangleMesh*>& a,
                const std::pair<std::string, TriangleMesh*>& b) {
                    float distanceA = glm::length(frameInfo.camera.getPosition() - a.second->getPosition());
                    float distanceB = glm::length(frameInfo.camera.getPosition() - b.second->getPosition());
                    return distanceA > distanceB; // Tri décroissant (back-to-front)
            }); 


            for (auto& object : transparentsObject) {
                auto material = SceneManager::getInstance()->getMaterialMap().find(object.first);

                vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                    2, // starting set
                    1, // number of sets
                    &(material->second->getDescriptorSet()[frameInfo.frameIndex]),
                    0,
                    nullptr
                );

                SimplePushConstantData push{};
                push.modelMatrix = object.second->getModel()->transform.mat4();
                push.normalMatrix = object.second->getModel()->transform.normalMatrix();

                vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
                object.second->bind(frameInfo.commandBuffer, frameInfo.frameIndex, pipelineLayout);
                object.second->draw(frameInfo.commandBuffer);

            }
    }

    void SimpleRenderSystem::render(FrameInfo& frameInfo) {
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
        drawTransparentBatch(frameInfo, SceneManager::getInstance()->getTransparentRenderingBatch());

    }
}
