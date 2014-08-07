#include "Window.h"
#include "Game.h"

using namespace df;

Window::Window() {
	window = glfwCreateWindow(800, 600, "DFEngine Window", NULL, NULL);
	init = window;
	
	if(init) {
		title = "DFEngine Window";
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetKeyCallback(window, KeyInputCallback);
		glfwSetMouseButtonCallback(window, MouseInputCallback);
		glfwSetWindowUserPointer(window, this);
	}
	
	is_active = true;
}

Window::~Window() {
	glfwDestroyWindow(window);
}

void Window::run(float delta_time) {
	if(!init)
		return;
	if(glfwWindowShouldClose(window)) {
		init = false;
		glfwDestroyWindow(window);
		return;
	}
	
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	mouseMoveEvent(Vec2d(mx, my));
	
	glClearColor(game->getBg().x, game->getBg().y, game->getBg().z, 1.0);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	SingleContainer::run(delta_time);
	glfwSwapBuffers(window);
}

void df::KeyInputCallback(GLFWwindow* window, int key, int scancode, int action, int modifiers) {
	Window* active_window = (Window*)glfwGetWindowUserPointer(window);
	active_window->keyEvent(key, scancode, action, modifiers);
}

void df::MouseInputCallback(GLFWwindow* window, int button, int action, int	modifiers) {
	Window* active_window = (Window*)glfwGetWindowUserPointer(window);
	double xpos = 0;
	double ypos = 0;
	glfwGetCursorPos(window, &xpos, &ypos);
	Vec2d position(xpos, active_window->requestSize().y - ypos);
	active_window->mouseEvent(position, button, action, modifiers);
}
