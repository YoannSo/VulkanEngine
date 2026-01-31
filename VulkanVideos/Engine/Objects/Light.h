#pragma once

#include <glm.hpp>
#include "GameObject.hpp"
#include <iostream>
namespace engine
{
	struct LightGPU
	{
		glm::vec4 position;   // xyz + type (w)
		glm::vec4 direction;  // xyz + unused
		glm::vec4 color;      // rgb + intensity
		glm::vec4 params;
	};

	enum class LightType
	{
		PointLight = 0,
		DirectionalLight = 1,
		SpotLight = 2
	};

	struct Light : public GameObject
	{
		glm::vec3 _color{ glm::vec3(1.0,1.0,1.0) };
		float _intensity{ 1.f };
		glm::vec3 _direction{ glm::vec3(0.0, -1.0, 0.0) };
		LightType _type{ LightType::PointLight };

		LightGPU getBuffer() const {
			LightGPU lightGpu{};
			lightGpu.position = glm::vec4(transform.translation, static_cast<float>(_type));
			lightGpu.direction = glm::vec4(_direction, 0.f);
			lightGpu.color = glm::vec4(_color, _intensity);
			lightGpu.params = glm::vec4(static_cast<float>(_type), 0.f, 0.f,0.f);
			return lightGpu;
		}
	};
}
