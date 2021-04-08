#include "Window.hpp"

Window::Window(const GLchar* title, const int& width, const int& height) : width(width), height(height)
{
	// Create window object
	this->window = glfwCreateWindow(this->width, this->height, title, nullptr, nullptr);
	if (this->window == nullptr)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return;
	}
	// Set current context for window
	glfwMakeContextCurrent(this->window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW\n";
		return;
	}

	// Define the viewport dimensions
	glViewport(0, 0, this->width, this->height);
}

Window::~Window()
{
	delete this->window;
}

const GLuint& Window::getWidth() const
{
	return this->width;
};

const GLuint& Window::getWHeight() const
{
	return this->height;
}

GLFWwindow* Window::getWindow() const
{
	return this->window;
}

void Window::setKeyCallBack(GLFWkeyfun callback)
{
	// Set key callback
	glfwSetKeyCallback(this->window, callback);
}