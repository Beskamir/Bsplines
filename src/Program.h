#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Geometry.h"
#include "InputHandler.h"
#include "RenderEngine.h"

class Program {

public:
	Program();
	void start();

private:
	GLFWwindow* window;
	RenderEngine* renderEngine;

	std::vector<Geometry*> geometryObjects;

	bool show_test_window;
	ImVec4 clear_color;

	static void error(int error, const char* description);
	void setupWindow();
	void mainLoop();
	void drawUI();

	void createTestGeometryObject();

	
	// Class variables for controlling the hypocycloid.
	float rotation = 0;
	float scale = 1;
	int step = 1;

	// Class variable for the hypocycloid
	// Geometry* hypocycloid;
	// Geometry* innerCircle;
	// Geometry* outerCircle;
	// Geometry* lastPoint;
	// Geometry* polynomialPoints;
	// Geometry* polynomialLine;

	glm::vec3 *mousePosition;

	ImVec4 lineColor;
};
