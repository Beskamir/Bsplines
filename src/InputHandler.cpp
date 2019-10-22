#include "InputHandler.h"

RenderEngine* InputHandler::renderEngine;
int InputHandler::mouseOldX;
int InputHandler::mouseOldY;
glm::vec3* InputHandler::mousePos;

// Must be called before processing any GLFW events
void InputHandler::setUp(RenderEngine* renderEngine, glm::vec3* pos) {
	InputHandler::renderEngine = renderEngine;
	InputHandler::mousePos = pos;
}

// Callback for key presses
void InputHandler::key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);
	}
}

// Callback for mouse button presses
void InputHandler::mouse(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
		mousePos->z = 1.0f;
	}
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_1) {
		mousePos->z = 0.0f;
	}
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_2) {
		mousePos->z = 2.0f;
	}
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_3) {
		mousePos->z = 3.0f;
	}
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_3) {
		mousePos->z = 0.0f;
	}
	// mousePos->x = mouseOldX;
	// mousePos->y = mouseOldY;
}

// Callback for mouse motion
void InputHandler::motion(GLFWwindow* window, double x, double y) {
	mouseOldX = x;
	mouseOldY = y;

	mousePos->x = mouseOldX;
	mousePos->y = mouseOldY;
}

// Callback for mouse scroll
void InputHandler::scroll(GLFWwindow* window, double x, double y) {
}

// Callback for window reshape/resize
void InputHandler::reshape(GLFWwindow* window, int width, int height) {
	renderEngine->setWindowSize(width, height);
}
