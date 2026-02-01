#include <Vulkan/Window.hpp>
#include <stdexcept>

#include <Vulkan/Device.hpp>
namespace engine {

	Window::Window(int w, int h, std::string name) :_width{ w }, _height{ h }, _windowName{ name }{
		initWindow();
		Device::setupInstance(*this);
	}
	Window::~Window() {
		glfwDestroyWindow(_window);//detroy our window
		glfwTerminate(); //terminate glfw
	}
	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create window surface");
	}
	void Window::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);// tell not to create opengl context 
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);//tell to not resize

		_window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);//make window,  first null parameter is for fullscreen mode
		glfwSetWindowUserPointer(_window, this);//register call back function, when the function is resie the function wil be call, 
		glfwSetFramebufferSizeCallback(_window, framebufferResizeCallBack);
	}
	void Window::framebufferResizeCallBack(GLFWwindow* window, int width, int height)
	{
		 auto lveWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	lveWindow->_frameBufferResized = true;
  lveWindow->_width = width;
  lveWindow->_height = height;
	}
}