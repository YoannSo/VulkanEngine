#pragma once
#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "lve_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <functional>
namespace lve {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f,1.f };
        glm::vec3 rotation{};

        void rotate(glm::vec3 p_rot,float p_dt);

        glm::mat4 mat4() const;
        glm::mat3 normalMatrix() const;
    };
    struct PointLightComponent {
        float lightIntensity=1.0f;
        
    };
    class GameObject {
    public:
        virtual ~GameObject() = default;
        GameObject();

       inline uint32_t getId() { return m_id; }
       inline void setUpdateFunction(std::function<void(GameObject*,float)> p_updateFunction) { m_updateFunction=p_updateFunction;}
       bool callUpdateFunction(float p_dt);


        TransformComponent transform;
    protected:

        uint32_t m_id;
        static uint32_t s_maxId;
        std::function<void(GameObject*,float)> m_updateFunction;


    };
}  // namespace lve
#endif