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

	std::vector<std::shared_ptr<Geometry>> geometryObjects;

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
	bool selectControlPoint();
	void moveActivePoint();
	void removeActivePoint();
	void updateActivePoint();
	int computeDelta(float uValue, int controlSize);
	void updateBsplineCurve();
	void updateDemoLines();
	void updateDemoPoint();

	// Class variables for controlling the hypocycloid.
	float rotation = 0;
	float scale = 1;
	int curveOrder = 2;
	int uIncrement = 10;
	bool removePoint = false;

	int activePointIndex = 0;

	// Geometry storage for b-splines
	std::shared_ptr<Geometry> controlPoints;
	std::shared_ptr<Geometry> activePoint;
	std::shared_ptr<Geometry> bsplineCurve;
	std::shared_ptr<Geometry> demoLines;
	std::shared_ptr<Geometry> demoPoint;

	std::vector<float> knots;

	std::shared_ptr<glm::vec3> mousePosition;

	ImVec4 lineColor;
};
