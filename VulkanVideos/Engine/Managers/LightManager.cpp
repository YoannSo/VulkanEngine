#include "LightManager.h"

void engine::LightManager::addPointLight(const glm::vec3& p_position, const glm::vec3& p_color, float p_intensity)
{
	m_lights.emplace_back();
	Light& light = m_lights.back();
	light._type = LightType::PointLight;
	light.transform.translation = p_position;
	light._color = p_color;
	light._intensity = p_intensity;
}

void engine::LightManager::addDirectionalLight(const glm::vec3& p_direction, const glm::vec3& p_color, float p_intensity)
{
	m_lights.emplace_back();
	Light& light = m_lights.back();
	light._type = LightType::DirectionalLight;
	light._direction = p_direction;
	light._color = p_color;
	light._intensity = p_intensity;
}

void engine::LightManager::addSpotLight(const glm::vec3& p_position, const glm::vec3& p_direction, const glm::vec3& p_color, float p_intensity, float p_cutOff, float p_outerCutOff)
{
	
}

const std::vector<engine::LightGPU> engine::LightManager::getLightsBufferData()const
{
	std::vector<LightGPU> lightBufferData;
	for (const auto& light : m_lights) {
		lightBufferData.emplace_back(light.getBuffer());
	}
	return lightBufferData;
}
