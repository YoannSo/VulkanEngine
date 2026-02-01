#include <Engine/Objects/GameObject.hpp>


namespace engine {

    uint32_t GameObject::s_maxId = 0;

    GameObject::GameObject() {
        m_id = ++s_maxId;
    }

    bool GameObject::callUpdateFunction(float p_dt)
    {
        if (m_updateFunction) {
            m_updateFunction(this,p_dt);
            return true;
        }
        return false;
    }

    void TransformComponent::rotate(glm::vec3 p_rot, float p_dt)
    {
        rotation += p_rot*p_dt;
    }

    glm::mat4 TransformComponent::mat4() const {//need to be imrpove bc use by frames for all bojects
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f} };
    }

    glm::mat3 TransformComponent::normalMatrix() const
    {
        // Correct normal matrix = transpose(inverse( modelMatrix )) upper-left 3x3.
        // Robust for non-uniform scale + rotation. We build the full mat4 then extract.
        glm::mat4 model = mat4();
        glm::mat4 invTrans = glm::transpose(glm::inverse(model));
        return glm::mat3(invTrans);
    }


   /* LveGameObject* LveGameObject::makePointLight(float intensity, float radius, glm::vec3 color)
    {
        LveGameObject* gameObj = LveGameObject::createGameObject();

        gameObj->color = color;
        gameObj->transform.scale.x = radius;
        gameObj->pointLight = std::make_unique<PointLightComponent>();
        gameObj->pointLight->lightIntensity = intensity;
        return gameObj;
    }
    */
}