#include "window.hpp"
#include <stdexcept>

#include "lve_device.hpp"
namespace lve {

	LveWindow::LveWindow(int w, int h, std::string name) :_width{ w }, _height{ h }, _windowName{ name }{
		initWindow();
		LveDevice::setupInstance(*this);
	}
	LveWindow::~LveWindow() {
		glfwDestroyWindow(_window);//detroy our window
		glfwTerminate(); //terminate glfw
	}
	void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create window surface");
	}
	void LveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);// tell not to create opengl context 
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);//tell to not resize

		_window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);//make window,  first null parameter is for fullscreen mode
		glfwSetWindowUserPointer(_window, this);//register call back function, when the function is resie the function wil be call, 
		glfwSetFramebufferSizeCallback(_window, framebufferResizeCallBack);
	}
	void LveWindow::framebufferResizeCallBack(GLFWwindow* window, int width, int height)
	{
		 auto lveWindow = reinterpret_cast<LveWindow *>(glfwGetWindowUserPointer(window));
	lveWindow->_frameBufferResized = true;
  lveWindow->_width = width;
  lveWindow->_height = height;
	}
}