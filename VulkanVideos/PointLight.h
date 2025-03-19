#pragma once
#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "GameObject.hpp"
namespace lve
{
	class PointLight : public GameObject
	{
	public:

		PointLight();
		PointLight(glm::vec3 p_color, float p_intensity, glm::vec3 p_position);
		~PointLight();



	private:
		glm::vec3 m_color{glm::vec3(1.0,1.0,1.0)};
		float m_intensity{ 1.f };
		glm::vec3 m_position{ glm::vec3(0.0,0.0,0.0) };
	};
}

#endif 

