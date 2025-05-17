#pragma once
#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm.hpp>
#include <string>

#include "memory"
#include "lve_texture.hpp"
#include "lve_buffer.hpp"
namespace lve {


	class Material
	{
	

		struct MaterialUBO {
			glm::vec4 ambientColor{ 0.f, 0.f, 0.f, 1.f };  // Ka (Ambient)
			glm::vec4 diffuseColor{ 1.f, 0.f, 0.f, 1.f };  // Kd (Diffuse)
			glm::vec4 specularColor{ 0.f, 0.f, 0.f, 1.f }; // Ks (Specular)
			glm::vec4 emissionColor{ 0.f, 0.f, 0.f, 1.f }; // Ke (Emission)
			glm::vec4 transmissionFilter{ 1.f, 1.f, 1.f, 1.f }; // Tf (Transmission Filter)

			float shininess{ 0.f };  // Ns (Shininess)
			float opacity{ 1.f };    // d (Opacity)
			float indexOfRefraction{ 1.f }; // Ni (IOR)
			float transparency{ 0.f }; // Tr (Transparency)

			int idDiffuseMap{ -1 };
			int idNormalMap{ -1 };
			int idSpecularMap{ -1 };
			int idShininessMap{ -1 };
			int idAmbientMap{ -1 };
			int idEmissionMap{ -1 };  // Ke Texture
			int idTransmissionMap{ -1 }; // Tf Texture

			int illuminationModel{ 0 }; // illum value

		};

	public:

		enum class EMaterialParameter {
			DIFFUSEMAP,
			AMBIENTMAP,
			SPECULARMAP,
			DIFFUSECOLOR,
			SPECULARCOLOR,
			SHININESS,
			OPACITY,
			REFRACTION,
			TRANSPARENCY,
			ILLUMINATIONMODEL
		};


		Material(std::string p_name);

		inline std::string getName() { return m_name; }

		void setMaterialParameter(EMaterialParameter p_materialParameter, glm::vec4 p_color);
		void setMaterialParameter(EMaterialParameter p_materialParameter, std::shared_ptr<LveTexture> p_texture);
		void setMaterialParameter(EMaterialParameter p_materialParameter,float p_value);


		std::vector<VkDescriptorImageInfo> getVkDescriptorImages();
		inline std::vector<VkDescriptorSet>& getDescriptorSet() { return m_materialDescriptorSet; }

		inline bool isTransparent() const { return m_isTransparent; }

		void setupDescriptorSet();

		MaterialUBO m_ubo{};

	private:
		const std::string m_name;
		bool m_isTransparent{ false };

		glm::vec3 m_ambient{ 0.f,0.f,0.f };
		glm::vec3 m_diffuse{ 0.f,0.f,0.f };
		glm::vec3 m_specular{ 0.f,0.f,0.f };
		float m_shininess{ 0.f };
		float shininess{ 0.f };  // Ns (Shininess)
		float opacity{ 1.f };    // d (Opacity)
		float indexOfRefraction{ 1.f }; // Ni (IOR)
		float transparency{ 0.f }; // Tr (Transparency)
		int illuminationModel{ 0 }; // illum value

		std::shared_ptr<LveTexture> m_ambientMap{ nullptr };
		std::shared_ptr<LveTexture> m_diffuseMap{ nullptr };
		std::shared_ptr<LveTexture> m_specularMap{ nullptr };
		std::shared_ptr<LveTexture> m_shininessMap{ nullptr };
		std::shared_ptr<LveTexture> m_normalMap{ nullptr };



		std::vector<LveBuffer*> m_uboBuffer;
		std::vector<VkDescriptorSet> m_materialDescriptorSet;

	};
}
#endif
