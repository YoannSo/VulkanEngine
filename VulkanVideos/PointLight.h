#pragma once
#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "GameObject.hpp"
namespace engine
{
	class PointLight : public GameObject
	{
	public:

		PointLight();
		PointLight(glm::vec3 p_color, float p_intensity, glm::vec3 p_position);
		~PointLight();

		glm::vec3 getColor() const { return m_color; }
		float getIntensity() const { return m_intensity; }

		void setColor(glm::vec3 p_color) { m_color = p_color; }
		void setIntensity(float p_intensity) { m_intensity = p_intensity; }

	private:
		glm::vec3 m_color{glm::vec3(1.0,1.0,1.0)};
		float m_intensity{ 1.f };
		glm::vec3 m_position{ glm::vec3(0.0,0.0,0.0) };
	};
}

#endif 

