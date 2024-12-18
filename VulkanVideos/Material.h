#pragma once
#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>
#include <string>

#include "memory"
#include "lve_texture.hpp"
#include "lve_buffer.hpp"
namespace lve {


	class Material
	{
	

		struct MaterialUBO {
			glm::vec4 ambientColor{ 0.f,0.f,0.f,0.f }; //vec4 to aligment
			glm::vec4 diffuseColor{ 0.f,0.f,0.f,0.f };
			glm::vec4 m_specular{ 0.f,0.f,0.f,0.f };
			float m_shininess{ 0.f };
			int idDiffuseMap{ -1 };
			int m_idNormalMap{ -1 };
			int m_idSpecularMap{ -1 };
			int m_idShininessMap{ -1 };
			int m_idAmbientMap{ -1 };

		};

	public:

		enum class EMaterialParameter {
			DIFFUSEMAP,
			DIFFUSECOLOR
		};


		Material(std::string p_name);

		inline std::string getName() { return m_name; }

		void setMaterialParameter(EMaterialParameter p_materialParameter, glm::vec4 p_color);
		void setMaterialParameter(EMaterialParameter p_materialParameter, std::shared_ptr<LveTexture> p_texture);


		std::vector<VkDescriptorImageInfo> getVkDescriptorImages();
		inline std::vector<VkDescriptorSet>& getDescriptorSet() { return m_materialDescriptorSet; }


		void setupDescriptorSet();

	private:
		const std::string m_name;

		glm::vec3 m_ambient{ 0.f,0.f,0.f };
		glm::vec3 m_diffuse{ 0.f,0.f,0.f };
		glm::vec3 m_specular{ 0.f,0.f,0.f };
		float m_shininess{ 0.f };

		std::shared_ptr<LveTexture> m_ambientMap{ nullptr };
		std::shared_ptr<LveTexture> m_diffuseMap{ nullptr };
		std::shared_ptr<LveTexture> m_specularMap{ nullptr };
		std::shared_ptr<LveTexture> m_shininessMap{ nullptr };
		std::shared_ptr<LveTexture> m_normalMap{ nullptr };

		MaterialUBO m_ubo{};


		std::vector<LveBuffer*> m_uboBuffer;
		std::vector<VkDescriptorSet> m_materialDescriptorSet;

	};
}
#endif
