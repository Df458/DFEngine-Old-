#include "Game.h"
#include "Window.h"
#include "Util.h"
#include "Viewport.h"
#include "Box.h"

using namespace df;

Game* df::game;

int main(int argc, char** argv) {
	if(!glfwInit())
		return -1;
	
	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	
	Window window;
	if(!window.exists()) {
		std::cerr << "Error: Couldn't create the main window!\n";
		glfwTerminate();
		return -1;
	}
	
	Viewport* view1 = new Viewport();
	view1->setFill(true);
	Box* box1 = new Box();
	box1->setOrientation(ORIENTATION_HORIZONTAL);
	
	window.prepareForDrawing();
	window.addChild(box1);
	box1->addChild(view1);
	
	glewExperimental = true;
	if(glewInit() != GLEW_OK)
		std::cerr << "Glew init failed!";
	
	game = new Game();
	game->init();
	game->setWindow(&window);
	glfwSetTime(0);

	while(window.exists()) {
		float dt = glfwGetTime() * FPS;
		glfwSetTime(0);
		game->run(dt);
		glfwPollEvents();
	}
	game->cleanup();
	delete game;
	delete box1;
	delete view1;
	
	glfwTerminate();
	return 0;
}
