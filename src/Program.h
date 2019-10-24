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

	void clearKnots();
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
	// Methods for controlling the control points
	void savePoints();
	void resetPoints();
	void addControlPoint(glm::vec3 oldPoint);
	glm::vec4 fixMousePoisiton() const;
	void addActivePoint();
	void updateControlPoints();
	bool selectControlPoint();
	void moveActivePoint() const;
	void removeActivePoint();
	void updateActivePoint();
	// Methods for controlling the resulting curves
	void clearCurve();
	int computeDelta(float &uValue);
	glm::vec3 deBoorAlg(int delta, float uValue);
	float deBoorAlgWeightsOnly(int delta, float uValue);
	void updateBsplineCurve();
	void deBoorAlgShow(int delta);
	void updateDemoLines();
	void updateDemoPoint();
	// Methods for controlling knots
	void createKnots();
	void createStandardKnots();
	void createUniformKnots();
	bool selectKnot();
	void moveKnot();
	void updateActiveKnot();


	// Class variables for controlling the hypocycloid.
	float rotation = 0;
	float scale = 1;
	float translation[2] = { 0, 0 };
	
	int curveOrder = 2;
	int uIncrement = 100;
	float demoU = 0;
	
	bool removePoint = false;
	bool drawCurve = true;
	bool drawPoints = true;
	bool drawDemoGeom = false;
	bool drawDemoPoint = false;

	bool standardKnots = true;
	bool uniformKnots = false;
	bool updateKnots = true;
	bool drawKnots = true;

	int activePointIndex = 0;
	int activeKnotIndex = 0;

	// Geometry storage for b-splines
	std::shared_ptr<Geometry> controlPoints;
	std::shared_ptr<Geometry> activePoint;
	std::shared_ptr<Geometry> bsplineCurve;
	std::shared_ptr<Geometry> demoLines;
	std::shared_ptr<Geometry> demoPoint;
	std::shared_ptr<Geometry> knotsRender;
	std::shared_ptr<Geometry> activeKnot;


	std::vector<glm::vec3> controlPointSave;
	std::vector<glm::vec3> activePointSave;

	std::vector<float> knots;

	std::vector<float> nurbValues = {1};

	std::shared_ptr<glm::vec3> mousePosition;

	ImVec4 lineColor;
};
