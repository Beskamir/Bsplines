#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
}

// Error callback for glfw errors
void Program::error(int error, const char* description) {
	std::cerr << description << std::endl;
}

// Called to start the program
void Program::start() {
	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	renderEngine = new RenderEngine(window);

	mousePosition = std::make_shared<glm::vec3>(0);

	InputHandler::setUp(renderEngine, mousePosition);
	mainLoop();
}

// Creates GLFW window for the program and sets callbacks for input
void Program::setupWindow() {
	glfwSetErrorCallback(Program::error);
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(1920, 1080, "CPSC 589 A2 - B-splines", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-sync on

	glfwSetKeyCallback(window, InputHandler::key);
	glfwSetMouseButtonCallback(window, InputHandler::mouse);
	glfwSetCursorPosCallback(window, InputHandler::motion);
	glfwSetScrollCallback(window, InputHandler::scroll);
	glfwSetWindowSizeCallback(window, InputHandler::reshape);

	// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

	const char* glsl_version = "#version 430 core";

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

// Creates an object from specified vertices - no texture. Default object is a 2D triangle.
void Program::createTestGeometryObject() {
	std::shared_ptr<Geometry> testObject = std::make_shared<Geometry>();

	testObject->verts.push_back(glm::vec3(-5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(0.f, 5.f, 0.f));
	renderEngine->assignBuffers(*testObject);
	renderEngine->updateBuffers(*testObject);
	geometryObjects.push_back(testObject);
}

void Program::createControlPoints() {
	controlPoints = std::make_shared<Geometry>();
	controlPoints->drawMode = GL_POINTS;
	renderEngine->assignBuffers(*controlPoints);
	geometryObjects.push_back(controlPoints);

}

void Program::createActivePoint() {
	activePoint = std::make_shared<Geometry>();
	activePoint->drawMode = GL_POINTS;
	activePoint->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	renderEngine->assignBuffers(*activePoint);
	geometryObjects.push_back(activePoint);
}

void Program::createBsplineCurve() {
	bsplineCurve = std::make_shared<Geometry>();
	renderEngine->assignBuffers(*bsplineCurve);
	geometryObjects.push_back(bsplineCurve);
}

void Program::createDemoLines() {
	demoLines = std::make_shared<Geometry>();
	demoLines->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	renderEngine->assignBuffers(*demoLines);
	geometryObjects.push_back(demoLines);
}

void Program::createDemoPoint() {
	demoPoint = std::make_shared<Geometry>();
	demoPoint->drawMode = GL_POINTS;
	demoPoint->color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	renderEngine->assignBuffers(*demoPoint);
	geometryObjects.push_back(demoPoint);
}

void Program::updateControlPoints() {
	controlPoints->color = glm::vec4(lineColor.x, lineColor.y, lineColor.z, lineColor.w);

	controlPoints->modelMatrix = glm::mat4(1.f);
	controlPoints->modelMatrix = glm::scale(controlPoints->modelMatrix, glm::vec3(scale));
	controlPoints->modelMatrix = glm::rotate(controlPoints->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	renderEngine->updateBuffers(*controlPoints);
}


void Program::addControlPoint(glm::vec3 oldPoint) {
	activePointIndex = controlPoints->verts.size();

	controlPoints->verts.emplace_back(oldPoint);
}

glm::vec3 Program::fixMousePoisiton() const {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::vec4 mousePosFix = glm::vec4(
		((mousePosition->x - (float)width / 2) / ((float)width / 2)) * 10 * (float)width / (float)height,
		(((float)height / 2 - mousePosition->y) / ((float)height / 2)) * 10,
		0.0f, 1.0f
	);
	// std::cout << mousePosFix.x << "," << mousePosFix.y << std::endl;

	mousePosFix = glm::inverse(activePoint->modelMatrix) * mousePosFix;
	return glm::vec3(mousePosFix.x, mousePosFix.y, 0);
}

void Program::addActivePoint() {

	// Boolean for catching just a single click
	mousePosition->z = 0;

	// Convert screen res to screen space
	const glm::vec3 mousePosFix = fixMousePoisiton();


	if (activePoint->verts.empty()) {
		addControlPoint(mousePosFix);
		activePoint->verts.emplace_back(glm::vec3(mousePosFix.x, mousePosFix.y, 0.f));
	}
	else {
		addControlPoint(mousePosFix);
		activePoint->verts[0] = (mousePosFix);
	}
}

bool Program::selectControlPoint() {
	// Convert screen res to screen space
	const glm::vec3 mousePosFix = fixMousePoisiton();
	for (int i = 0; i < controlPoints->verts.size(); i++) {
		if (glm::distance(mousePosFix, controlPoints->verts[i]) < 0.35f) {
			activePoint->verts[0] = controlPoints->verts[i];
			activePointIndex = i;
			return true;
		}
	}
	return false;
}

void Program::moveActivePoint() {
	const glm::vec3 mousePosFix = fixMousePoisiton();
	activePoint->verts[0] = mousePosFix;
	controlPoints->verts[activePointIndex] = mousePosFix;
	renderEngine->updateBuffers(*controlPoints);
}

void Program::removeActivePoint() {
	const glm::vec3 mousePosFix = fixMousePoisiton();

	if(controlPoints->verts.empty()) {
		return; // If array is already empty, return so we don't crash
	}

	// Otherwise remove the active point and assign a new active point
	controlPoints->verts.erase(controlPoints->verts.begin() + activePointIndex);

	if(controlPoints->verts.empty()) {
		activePoint->verts.clear();
		activePointIndex = -1;
	}
	else {
		activePointIndex = controlPoints->verts.size() - 1;
		activePoint->verts[0] = controlPoints->verts[activePointIndex];
	}
	renderEngine->updateBuffers(*controlPoints);
}


void Program::updateActivePoint() {
	// controlPoints->color = glm::vec4(lineColor.x, lineColor.y, lineColor.z, lineColor.w);

	activePoint->modelMatrix = glm::mat4(1.f);
	activePoint->modelMatrix = glm::scale(activePoint->modelMatrix, glm::vec3(scale));
	activePoint->modelMatrix = glm::rotate(activePoint->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	// Select and modify control points
	if(mousePosition->z == 1) {
		mousePosition->z = selectControlPoint() ? 11 : 0;
	}
	if (mousePosition->z == 11) {
		moveActivePoint();
	}

	// Add new control points
	if (mousePosition->z == 2) {
		addActivePoint();
		mousePosition->z = 0;
	}

	if (removePoint) {
		removeActivePoint();
		removePoint = false;
	}
	
	renderEngine->updateBuffers(*activePoint);
}

int Program::computeDelta(float uValue) {
	for (int i = 0; i < controlPoints->verts.size()+curveOrder; ++i)	
	{
		if(uValue>=knots[i] && uValue<knots[i+1]){
			// std::cout << uValue << ":" << i << ":" << knots[i] << std::endl;
			return i;
		}
	}
	return -1;
}

// glm::vec3 Program::deBoorAlg(int delta, float uValue){
// 	std::vector<glm::vec3> contributorPoints;
// 	float curveDegree = curveOrder - 1;
// 	contributorPoints.reserve(curveOrder);
// 	for (int i = 0; i < curveOrder; i++)
// 	{
// 		contributorPoints.push_back(controlPoints->verts[i + delta - curveDegree]);
// 	}
//
// 	for (int r = 1; r < curveOrder; r++)
// 	{
// 		for (int j = 0; j < r-1; j--)
// 		{
// 			float knot1 = knots[j + delta - curveDegree];
// 			float knot2 = knots[j + 1 + delta - r];
// 			float knot3 = knots[j + delta - curveDegree];
// 			float alpha = (uValue - knot1)  /  (knot2 - knot3);
// 			contributorPoints[j] = (1-alpha) * contributorPoints[j - 1] + alpha * contributorPoints[j];
// 		}
// 	}
// 	return contributorPoints[curveDegree];
//
// }

 glm::vec3 Program::deBoorAlg(int delta, float uValue){
 	std::vector<glm::vec3> contributorPoints;
 	// float curveDegree = curveOrder - 1;
 	contributorPoints.reserve(curveOrder);
 	for (int i = 0; i < curveOrder; i++)
 	{
		// int index = 
 	// 	if(index >= controlPoints->verts.size())
 	// 	{
		// 	index = controlPoints->verts.size() - 1;
 	// 	}
		contributorPoints.push_back(controlPoints->verts[delta - i]);
 	}

	for (int r = curveOrder; r >= 2; r--)
	{
		int i = delta;
		for (int s = 0; s <= r-2; s++)
		{
			float omega = (uValue - knots[i]) / (knots[i + r - 1] - knots[i]);
			contributorPoints[s] = (omega * contributorPoints[s]) + ((1 - omega)*contributorPoints[s + 1]);
			i--;
		}
	}

 	return contributorPoints[0];
 }

void Program::createKnots(){
	knots.clear();
	for (int i = 0; i < curveOrder - 1; ++i) {
		knots.push_back(0);
	}
	const float knotSpacing = 1.f / float(controlPoints->verts.size() - curveOrder + 1);
	for (float i = 0; i <= 1; i += knotSpacing) {
		knots.push_back(i);
	}
	for (int i = 0; i < curveOrder - 1; ++i)
	{
		knots.push_back(1);
	}
	std::cout << "Knots: ";
	for (float knot : knots)
	{
		std::cout << knot << ",";
	}
	std::cout << std::endl;
}


void Program::updateBsplineCurve() {
	// TODO: algorithm to generate b-spline curve
	// checks and preprocessing
	if(controlPoints->verts.size()<curveOrder) { return; }
	// int controlSize = controlPoints->verts.size() - 1;
	bsplineCurve->verts.clear();

	createKnots();

	// Iterate through u values and generate curve
	float u = knots[curveOrder - 1];
	while (u <= 1) 
	{
		// Get the index of the control point that matters
		int delta = computeDelta(u);
		if(delta<0) { return; }
		// std::cout << "Delta: " << delta << std::endl;
		// Calculate the point on the curve
		bsplineCurve->verts.push_back(deBoorAlg(delta,u));
		renderEngine->updateBuffers(*bsplineCurve);
		u += (1 / (float)uIncrement);
	}
}

void Program::updateDemoLines() {
	// Todo: create lines to show bspline curve generation graphically
}

void Program::updateDemoPoint() {
	// Todo: current u value
}


void Program::drawUI() {
	// Start ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Setup all the buttons for ImGui interaction
	{
		ImGui::Begin("UI Controls");

		float fontSize = 1.75f;
		ImGui::SetWindowFontScale(fontSize);

		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::ColorEdit4("line color", (float*)&lineColor);
		
		ImGui::DragFloat("rotation", (float*)&rotation, 0.1f);
		ImGui::DragFloat("scale factor", (float*)&scale, 0.001f);

		ImGui::DragInt("order (k value)", (int*)&curveOrder, 1);
		if (curveOrder < 2) {
			curveOrder = 2;
		}
		ImGui::DragInt("resolution (u increment)", (int*)&uIncrement, 1);
		if (uIncrement < 1) {
			uIncrement = 1;
		}

		if(ImGui::Button("remove point")) {
			removePoint = true;
		}


		ImGui::End();
	}
}

// Main loop
void Program::mainLoop() {
	
	// Our state
	show_test_window = false;
	clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	lineColor = ImVec4(1.0f, 1.0f, 0.0f, 1.00f);

	// createTestGeometryObject();
	createActivePoint();
	createControlPoints();
	createBsplineCurve();
	createDemoLines();
	createDemoPoint();

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		updateActivePoint();
		updateControlPoints();
		updateBsplineCurve();
		updateDemoLines();
		updateDemoPoint();

		drawUI();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		renderEngine->render(geometryObjects, glm::mat4(1.f));
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	delete renderEngine;
	glfwTerminate();
}
