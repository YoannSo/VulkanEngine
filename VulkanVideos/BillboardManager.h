#pragma once
#include <glm.hpp>
#include <vector>
namespace engine::managers {
	class BillboardManager
	{

	public:
		struct BillboardCPU {
			glm::vec4 position_size;// w: size
			glm::vec4 color;
			glm::uvec4 params; // x: textureID, y: type, z: unused, w: unused
			BillboardCPU(const glm::vec4& p_position_size, const glm::vec4& p_color, const glm::uvec4& p_params)
				: position_size(p_position_size), color(p_color), params(p_params) {
			}
		};;

	public:
		void addBillboard(const glm::vec3& p_position, float p_size, const glm::vec4& p_color, uint32_t p_type);
		const std::vector<BillboardCPU>& getBillboardsBufferData() const
		{
			return m_billboards;
		}
		void clearBillboards()
		{
			m_billboards.clear();
		}

	private:
		std::vector<BillboardCPU> m_billboards;
	};
}


