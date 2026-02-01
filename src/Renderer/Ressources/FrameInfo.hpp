#pragma once

#include <Engine/Objects/Camera.hpp>
#include<Vulkan/Descriptor.hpp>
#include <Engine/Objects/GameObject.hpp>
#include <Engine/Managers/SceneManager.h>
#include <vulkan/vulkan.h>
namespace engine {
#define MAX_LIGHTS 10

	struct ligthTemp {
		glm::vec4 position{};//ingore w, w just for aligment
		glm::vec4 color{};//w inteisty
		};


	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
	};
}