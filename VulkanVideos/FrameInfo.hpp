#pragma once

#include "Camera.hpp"
#include "Descriptor.hpp"
#include "GameObject.hpp"
#include "SceneManager.h"
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