#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "RenderEngine.h"

class InputHandler {

public:
	static void setUp(RenderEngine* renderEngine, std::shared_ptr<glm::vec3> pos);

	static void key(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse(GLFWwindow* window, int button, int action, int mods);
	static void motion(GLFWwindow* window, double x, double y);
	static void scroll(GLFWwindow* window, double x, double y);
	static void reshape(GLFWwindow* window, int width, int height);


private:
	static RenderEngine* renderEngine;
	static std::shared_ptr<glm::vec3> mousePos;

	static int mouseOldX;
	static int mouseOldY;
};
