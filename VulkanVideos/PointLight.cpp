#include "PointLight.h"
namespace lve
{
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
