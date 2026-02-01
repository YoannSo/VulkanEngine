#pragma once
#define GLFW_INCLUDE_VULKAN //said that we want to include vulkan
#include <glfw3.h>
#include <string>
namespace engine{

	class Window {

	public:
		Window(int w, int h, std::string name);
		~Window();

		Window(const Window&) = delete; // delete copy constructor
		Window& operator=(const Window&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed

		bool shouldClose(){ return glfwWindowShouldClose(_window); };
		bool wasWindowResized() { return _frameBufferResized; };
		void resetWindowResizedFlag() { _frameBufferResized = false; };
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		VkExtent2D getExtent(){ return{static_cast<uint32_t>(_width),static_cast<uint32_t>(_height)}; };
		GLFWwindow* getGLFWWindow()const { return _window;  }
	private:
		void initWindow();
		static void framebufferResizeCallBack(GLFWwindow* window, int width, int height);
		 int _width;
		 int _height;
		 bool _frameBufferResized=false;

		std::string _windowName;
		GLFWwindow* _window;
	};
}
