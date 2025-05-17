#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm.hpp>

#include "define.hpp"
#include "GameObject.hpp"

namespace lve {
	class Camera : public GameObject {

	public:
		Camera();
		~Camera()=default;

	};
}
#endif
