#pragma once

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
    class LveGameObject {
    public:

        static LveGameObject* createGameObject() {
            static uint32_t currentId = 0;
            return new LveGameObject{ currentId++ };
        }
        static LveGameObject* makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));



        uint32_t getId() { return id; }

        glm::vec3 color{};
        TransformComponent transform{};

        std::shared_ptr<LveModel> model= nullptr;
        std::shared_ptr<Model> _model= nullptr;
        std::shared_ptr<PointLightComponent> pointLight = nullptr;
    private:
        LveGameObject(uint32_t objId) : id{ objId } {}

        uint32_t id;
    };
}  // namespace lve