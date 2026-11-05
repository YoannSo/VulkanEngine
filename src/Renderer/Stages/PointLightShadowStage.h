#pragma once
#include <array>
#include <glm.hpp>
#include <memory>

#include <Engine/Managers/SceneManager.h>
//#include <Renderer/RenderSystems/PointLightShadowRS.h>
#include <Renderer/Stages/RenderStage.h>
#include <Vulkan/Texture.hpp>

namespace engine::stages
{

    class PointLightShadowStage : public RenderStage
    {
    public:
        PointLightShadowStage(const engine::Device& p_device, const SwapChain& p_swapchain, const SceneManager& p_sceneManager);

        // Doit être appelé chaque frame, avant record(), pour indiquer où est la lumière à ombrer.
        void setLightPosition(const glm::vec3& p_position)
        {
            m_lightPosition = p_position;
        }

        VkImageView getShadowCubemapView() const
        {
            return m_shadowCubemap->getImageView();
        }

        // Override : la base RenderStage::record() ne fait qu'UN SEUL begin/end render pass.
        // Ici on a besoin de 6 passes (une par face), donc on réimplémente record() entièrement.
        void record(const FrameInfo& p_frameInfo, int p_currentSwapchainImage);

    protected:
        void createRenderPass() override;
        void createFrameBuffers() override;
        std::vector<VkClearValue> getClearValues() override;

    private:
        std::array<glm::mat4,6> getFaceViewMatrices() const; // une par face, calculées depuis m_lightPosition
        glm::mat4 getFaceProjectionMatrix() const;         // perspective 90°, identique pour les 6 faces

    private:
        static constexpr uint32_t SHADOW_MAP_RESOLUTION = 1024;
        static constexpr float SHADOW_NEAR = 0.1f;
        static constexpr float SHADOW_FAR = 25.0f; // portée de la point light

        std::shared_ptr<Texture> m_shadowCubemap; // format R32_SFLOAT, isCubemap = true
        std::shared_ptr<Texture> m_shadowDepth;   // format D32_SFLOAT, classique, partagé entre les 6 faces

        std::array<VkFramebuffer, 6> m_faceFrameBuffers{};

        glm::vec3 m_lightPosition{0.f, 0.f, 0.f};
    };
} // namespace engine::stages