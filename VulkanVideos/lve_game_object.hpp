#pragma once
#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "lve_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "model.hpp"
// std
#include <memory>
namespace lve {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f,1.f };
        glm::vec3 rotation;
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };
    struct PointLightComponent {
        float lightIntensity=1.0f;
        
    };
    class GameObject {
    public:

        virtual ~GameObject() = default;
        GameObject();

        uint32_t getId() { return m_id; }
        glm::vec3 color{};

        TransformComponent transform;
    private:

        uint32_t m_id;
        static uint32_t s_maxId;
    };
}  // namespace lve
#endif