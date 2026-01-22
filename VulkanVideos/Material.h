#pragma once
#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <glm.hpp>

#include "lve_texture.hpp"
#include "lve_buffer.hpp"
namespace lve {


	class Material
	{
	
	public:
		struct MaterialGPU {
			glm::vec4 ambien { 0.f,0.f,0.f,0.f };
			glm::vec4 diffuseColor{ 0.f,0.f,0.f,0.f };
			glm::vec4 specular{ 0.f,0.f,0.f,0.f };
			glm::vec4 emission{ 0.f,0.f,0.f,0.f };
			glm::vec4 transmissionFilter{ 0.f,0.f,0.f,0.f };

			glm::vec4 params1{ 0.f,1.f,0.f,0.f }; // shininess, opacity, transparency, ior
			glm::ivec4 maps1{ -1,-1,-1,-1 };  // diffuse, normal, specular, shininess
			glm::ivec4 maps2{ -1,-1,-1,-1 };  // ambient, emission, transmission, illum
		};


	public:

		enum class EMaterialParameter {
			DIFFUSEMAP,
			AMBIENTMAP,
			SPECULARMAP,
			NORMALMAP,
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
		void setMaterialParameter(EMaterialParameter p_materialParameter, uint32_t p_textureID);
		void setMaterialParameter(EMaterialParameter p_materialParameter,float p_value);

		inline bool isTransparent() const { return m_isTransparent;}

		inline const MaterialGPU& getMaterialBuffer() const { return m_gpuBuffer; }

	private:
		const std::string m_name;
		bool m_isTransparent{ false };

		glm::vec3 m_ambient{ 0.f,0.f,0.f };
		glm::vec3 m_diffuse{ 0.f,0.f,0.f };
		glm::vec3 m_specular{ 0.f,0.f,0.f };
		float m_shininess{ 0.f };
		float m_opacity{ 1.f };    // d (Opacity)
		float m_indexOfRefraction{ 1.f }; // Ni (IOR)
		float m_transparency{ 0.f }; // Tr (Transparency)
		int m_illuminationModel{ 0 }; // illum value

		uint32_t m_idDiffuseMap{ 0 };
		uint32_t m_idSpecularMap{ 0 };
		uint32_t m_idAmbientMap{ 0 };
		uint32_t m_idShininessMap{ 0 };
		uint32_t m_idNormalMap{ 0 };

		MaterialGPU m_gpuBuffer;

	};
}
#endif
