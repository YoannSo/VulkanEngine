#pragma once

#include "lve_camera.hpp"
#include "lve_descriptor.hpp"
#include "GameObject.hpp"
#include "SceneManager.h"
#include <vulkan/vulkan.h>
namespace lve {
#define MAX_LIGHTS 10

	struct ligthTemp {
		glm::vec4 positions{};//ingore w, w just for aligment
		glm::vec4 color{};//w inteisty
		};
	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };//cam pos last colum
		glm::vec4 ambientLightColor{ 1.f,1.f,1.f,0.02f };
		ligthTemp pointsLights[MAX_LIGHTS];
		int numLights;
	};


	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LveCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}