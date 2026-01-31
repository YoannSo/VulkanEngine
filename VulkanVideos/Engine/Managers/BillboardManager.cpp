#include "BillboardManager.h"

namespace engine::managers {
	void BillboardManager::addBillboard(const glm::vec3& p_position, float p_size, const glm::vec4& p_color, uint32_t p_type)
	{
		m_billboards.emplace_back(glm::vec4{p_position, p_size}, p_color, glm::uvec4{p_size, p_type, 0, 0});
	}
}