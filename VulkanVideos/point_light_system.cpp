#include "point_light_system.hpp"
#include <stdexcept>
#include <array>
#include <glm/gtc/constants.hpp>
#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1
#include <glm/glm.hpp>
#include <map>
namespace lve {
	struct PointLightPushConstants {//couble be rremplaced by indexed value in the globalUbo => only 1 draw call
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};
	PointLighRenderSystem::PointLighRenderSystem( VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	PointLighRenderSystem::~PointLighRenderSystem() {
		vkDestroyPipelineLayout(LveDevice::getInstance()->getDevice(), pipelineLayout, nullptr);
	}



	void PointLighRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);


		std::vector<VkDescriptorSetLayout> descriptorSetLayout{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; //empyt layout so 0 
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size()); //empty so 0
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data(); // pass date, autre que vertex data to vertex and framgnet (texture/uniform)
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // small amount of data, 

		if (vkCreatePipelineLayout(LveDevice::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void PointLighRenderSystem::createPipeline(VkRenderPass renderPass)
	{

		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(
			pipelineConfig);
		LvePipeline::enableAlphaBlinding(
			pipelineConfig);
		pipelineConfig.attributDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		// take swap chain w and h bc dont necessarly match the window,
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>( "shaders/point_light.vert.spv", "shaders/point_light.frag.spv", pipelineConfig);
	}



	void PointLighRenderSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
	{
		auto rotateLight = glm::rotate( //
			glm::mat4(1.f),//indeity matrix
			frameInfo.frameTime,// circle equal size of slice, rotate each point incremently to create a ring of light
			{ 0.f, -1.f, 0.f });//up vector

		int lightIndex = 0;
		for (auto& kv : SceneManager::getInstance()->getSceneObjects()) {
			auto& obj = kv.second;
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