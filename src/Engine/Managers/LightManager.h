#pragma once
#include <string>
#include <iostream>

#include <Utils/define.hpp>
#include <Engine/Objects/Light.h>
#include <Utils/DirtyFlag.h>
namespace engine {
    class LightManager {
    public:

		void addPointLight(const glm::vec3& p_position, const glm::vec3& p_color, float p_intensity);
		void addDirectionalLight(const glm::vec3& p_direction, const glm::vec3& p_color, float p_intensity);
		void addSpotLight(const glm::vec3& p_position, const glm::vec3& p_direction, const glm::vec3& p_color, float p_intensity, float p_cutOff, float p_outerCutOff);
		const std::vector<Light>& getLights() const { return m_lights; }
        const std::vector<LightGPU> getLightsBufferData()const;
		uint32_t getLightCount() const { return static_cast<uint32_t>(m_lights.size()); }
        void setDirectionalLightDirection(const glm::vec3& p_direction);
        glm::vec3 getDirectionalLightDirection() const;

        void markDirty()
        {
            m_dirtyFlag.markDirty();
        }

        bool consumeDirty()
        {
            return m_dirtyFlag.consume();
        }

    public:
        static const uint16_t MAX_LIGHT_IN_SCENE{ 100 };

    private:
        DirtyFlag m_dirtyFlag;
        std::vector<Light> m_lights;
    };
}