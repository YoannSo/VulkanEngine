#include "geometry_pass_render_system.h"

namespace lve {
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f }; // offset homegous 
        glm::mat4 normalMatrix{ 1.f };
    };


    GeometryPassRenderSystem::GeometryPassRenderSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : RenderSystem(renderPass, std::vector<VkDescriptorSetLayout>{ globalSetLayout }, sizeof(SimplePushConstantData), "shaders/point_light.vert.spv", "shaders/point_light.frag.spv")  // call base class constructor
    {
    }
    GeometryPassRenderSystem::~GeometryPassRenderSystem() {
    }

}
