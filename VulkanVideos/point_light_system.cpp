#include "point_light_system.hpp"

namespace lve {
	struct PointLightPushConstants {//couble be rremplaced by indexed value in the globalUbo => only 1 draw call
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

	PointLighRenderSystem::PointLighRenderSystem( VkRenderPass renderPass, std::vector<VkDescriptorSetLayout>& globalSetLayout)
		: RenderSystem()  // call base class constructor
	{
		uint32_t pushConstantSize = sizeof(PointLightPushConstants);
		init(renderPass, globalSetLayout, pushConstantSize, "shaders/point_light.vert.spv", "shaders/point_light.frag.spv", true);

	}
    PointLighRenderSystem::~PointLighRenderSystem() {  
    }

	void PointLighRenderSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
	{
		auto rotateLight = glm::rotate( //
			glm::mat4(1.f),//indeity matrix
			frameInfo.frameTime,// circle equal size of slice, rotate each point incremently to create a ring of light
			{ 0.f, -1.f, 0.f });//up vector

		int lightIndex = 0;
		for (auto& obj : SceneManager::getInstance()->getLightMap()) {
			//if (obj->pointLight == nullptr) continue;


			assert(lightIndex < MAX_LIGHTS && "Points light xceed maximum number");
			//update light pos
			obj->transform.translation = glm::vec3(rotateLight * glm::vec4(obj->transform.translation, 1.f));

			ubo.pointsLights[lightIndex].position = glm::vec4(obj->transform.translation, 1.f);
			//ubo.pointsLights[lightIndex].color = glm::vec4(obj->color, obj->pointLight->lightIntensity);
			lightIndex++;
		}
		ubo.numLights = lightIndex;
	}

	void PointLighRenderSystem::render(FrameInfo& frameInfo)
	{

		std::map<float, PointLight*> sorted;

		for (PointLight* pointLight : SceneManager::getInstance()->getLightMap()) {
			auto offset = frameInfo.camera.getPosition() - pointLight->transform.translation;
			float distSquared = glm::dot(offset, offset);
			sorted[distSquared] = pointLight;
		}

		lvePipeline->bind(frameInfo.commandBuffer);


		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0,//strating sert
			1,//number of set
			&frameInfo.globalDescriptorSet,
			0,
			nullptr
		);


		// iterate through sorted lights in reverse order
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
			// use game obj id to find light object
			PointLight* obj = it->second;
			//if (obj->pointLight == nullptr) continue;

			PointLightPushConstants push{};

			push.position = glm::vec4(obj->transform.translation, 1.f);
			push.color = glm::vec4(obj->getColor(), obj->getIntensity());
			push.radius = obj->transform.scale.x;
			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants), &push);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);


		}


	}
}