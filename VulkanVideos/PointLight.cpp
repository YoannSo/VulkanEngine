#include "PointLight.h"
namespace lve
{
	PointLight::PointLight()
	{
		m_color = glm::vec3(1.0, 1.0, 1.0);
		m_intensity = 1.f;
		m_position = glm::vec3(0.0, 0.0, 0.0);
	}
	PointLight::PointLight(glm::vec3 p_color, float p_intensity, glm::vec3 p_position)
	{
		m_color = p_color;
		m_intensity = p_intensity;
		m_position = p_position;
	}
	PointLight::~PointLight()
	{
	}
}
