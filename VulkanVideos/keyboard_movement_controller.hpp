#pragma once

#include "Camera.h"
#include "window.hpp"
#include <limits>

#include "SceneManager.h"

namespace lve {
	class KeyBoardMovementController {
	public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;

			int normalSwitch = GLFW_KEY_N;
			int lightningSwitch = GLFW_KEY_L;
        };
        void moveInPLaneXZ(GLFWwindow* window, float dt, Camera* gameObject);
        void handleAction(GLFWwindow* p_window);
        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.5f };

		bool m_normalPressed = 0;
        bool m_lightPressed = 0;
		SceneManager* m_sceneManager = SceneManager::getInstance();

	};
}