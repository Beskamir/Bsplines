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

	// Initialize all the geometry pointers
	void createControlPoints();
	void createActivePoint();
	void createBsplineCurve();
	void createDemoLines();
	void createDemoPoint();

	// Use the geometry pointers and fill them with relavent data
	void addControlPoint(glm::vec3 oldPoint);
	glm::vec3 fixMousePoisiton() const;
	void addActivePoint();

	void updateControlPoints();
	void selectControlPoint();
	void updateActivePoint();
	void updateBsplineCurve();
	void updateDemoLines();
	void updateDemoPoint();

	// Class variables for controlling the hypocycloid.
	float rotation = 0;
	float scale = 1;
	int curveOrder = 1;
	int uIncrement = 1;

	// Geometry storage for b-splines
	Geometry* controlPoints;
	Geometry* activePoint;
	Geometry* bsplineCurve;
	Geometry* demoLines;
	Geometry* demoPoint;


	glm::vec3 *mousePosition;

	ImVec4 lineColor;
};
