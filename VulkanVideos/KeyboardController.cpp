#include "KeyboardController.hpp"
namespace engine {

	void KeyBoardMovementController::moveInPLaneXZ(
		GLFWwindow* window, float dt, Camera* gameObject) {
		glm::vec3 rotate{ 0 };
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) { // only apply rotateion when rotate is not 0, not good idea to compare float with 0
			gameObject->transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		// limit pitch values between about +/- 85ish degrees
		gameObject->transform.rotation.x = glm::clamp(gameObject->transform.rotation.x, -1.5f, 1.5f);
		gameObject->transform.rotation.y = glm::mod(gameObject->transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject->transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			gameObject->transform.translation += moveSpeed * dt * glm::normalize(moveDir);
		}
	}

	void KeyBoardMovementController::handleAction(GLFWwindow* window) {

		// ===== NORMAL DEBUG =====
		if (glfwGetKey(window, keys.normalSwitch) == GLFW_PRESS && !m_normalPressed) {
			std::cout << "avant Normal toggled: " << m_sceneManager->_showNormal << std::endl;
			m_sceneManager->_showNormal = !m_sceneManager->_showNormal;
			m_normalPressed = true;

			std::cout << "Normal toggled: " << m_sceneManager->_showNormal << std::endl;
		}

		if (glfwGetKey(window, keys.normalSwitch) == GLFW_RELEASE) {
			m_normalPressed = false;
		}


		// ===== LIGHTING DEBUG =====
		if (glfwGetKey(window, keys.lightningSwitch) == GLFW_PRESS && !m_lightPressed) {
			m_sceneManager->_showLighning = !m_sceneManager->_showLighning;
			m_lightPressed = true;
		}

		if (glfwGetKey(window, keys.lightningSwitch) == GLFW_RELEASE) {
			m_lightPressed = false;
		}
	}

}