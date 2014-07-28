#ifndef DF_WINDOW
#define DF_WINDOW
#include "Util.h"
#include "SingleContainer.h"

namespace df {

void KeyInputCallback(GLFWwindow*, int, int, int, int);
void MouseInputCallback(GLFWwindow*, int, int, int);

class Window : public SingleContainer {
public:
	Window();
	~Window();
	std::string getTitle() { return title; }
	bool setTitle(std::string n_title) { 
		if(init) { 
			title = n_title;
			glfwSetWindowTitle(window, title.c_str());
		}
		return init;
	}
	bool exists() { return init; }
	
	void prepareForDrawing() { glfwMakeContextCurrent(window); }
	Widget* getChild() { return child; }
	virtual Vec2d calculatePosition(Widget*) { return Vec2d(0, requestSize().y); }
	virtual Vec2d calculateSize(Widget*) {
		int width; 
		int height; 
		glfwGetFramebufferSize(window, &width, &height); 
		return Vec2d(width, height);
	}
	virtual Vec2d requestSize() {
		int width; 
		int height; 
		glfwGetFramebufferSize(window, &width, &height); 
		return Vec2d(width, height);
	}
	virtual Vec2d requestPosition() {
		return Vec2d(0, 0);
	}
	virtual void keyEvent(int key, int scancode, int action, int modifiers) {
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		SingleContainer::keyEvent(key, scancode, action, modifiers);
	}
	virtual void mouseEvent(Vec2d position, int button, int action, int	modifiers) {
		SingleContainer::mouseEvent(position, button, action, modifiers);
	}
	void run(float delta_time);
protected:
	bool init = false;
	GLFWwindow* window = NULL;
	std::string title = "";
};

}
#endif
