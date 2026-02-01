#include <Engine/Objects/Material.h>

namespace engine {

	Material::Material(std::string p_name): m_name(p_name)
	{
	}
	void Material::setMaterialParameter(EMaterialParameter p_materialParameter, uint32_t p_textureID) {

		switch (p_materialParameter) {
			case EMaterialParameter::DIFFUSEMAP:
				m_idDiffuseMap = p_textureID;
				m_gpuBuffer.maps1.x = p_textureID;
				break;
			case EMaterialParameter::SPECULARMAP:
				m_idSpecularMap = p_textureID;
				m_gpuBuffer.maps1.z = p_textureID;
				break;
			case EMaterialParameter::AMBIENTMAP:
				m_idAmbientMap = p_textureID;
				m_gpuBuffer.maps2.x = p_textureID;
				break;
			case EMaterialParameter::NORMALMAP:
				m_idNormalMap = p_textureID;
				m_gpuBuffer.maps1.y = p_textureID;
				break;
		}
	}
	void Material::setMaterialParameter(EMaterialParameter p_materialParameter, glm::vec4 p_color) {
		switch (p_materialParameter) {
		case EMaterialParameter::DIFFUSECOLOR:
			m_diffuse = p_color;
			m_gpuBuffer.diffuseColor = p_color;
			break;
		case EMaterialParameter::SPECULARCOLOR:
			m_specular = p_color;
			m_gpuBuffer.specular = p_color;
			break;
		}
	}
	void Material::setMaterialParameter(EMaterialParameter p_materialParameter, float p_value) {
		switch (p_materialParameter) {
		case EMaterialParameter::SHININESS:
			m_shininess = p_value;
			m_gpuBuffer.params1.x = p_value;
			break;
		case EMaterialParameter::OPACITY:
			if(p_value <1.f)
				m_isTransparent = true;
			m_opacity = p_value;
			m_gpuBuffer.params1.y = p_value;
			break;
		case EMaterialParameter::REFRACTION:
			m_indexOfRefraction = p_value;
			m_gpuBuffer.params1.w = p_value;
			break;
		case EMaterialParameter::TRANSPARENCY:
			m_transparency = p_value;
			m_gpuBuffer.params1.z = p_value;
			break;
		case EMaterialParameter::ILLUMINATIONMODEL:
			m_illuminationModel = p_value;
			m_gpuBuffer.maps2.w = static_cast<int>(p_value);
			break;
			
		}
	}

}

