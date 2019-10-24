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

void Program::clearKnots() {
	knotsRender->verts.clear();
	activeKnot->verts.clear();
	renderEngine->updateBuffers(*knotsRender);
	renderEngine->updateBuffers(*activeKnot);
}

void Program::clearCurve() {
	bsplineCurve->verts.clear();
	demoPoint->verts.clear();
	demoLines->verts.clear();
	renderEngine->updateBuffers(*bsplineCurve);
	renderEngine->updateBuffers(*demoPoint);
	renderEngine->updateBuffers(*demoLines);
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
	controlPoints->color = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
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
	demoLines->drawMode = GL_LINES;
	demoLines->color = glm::vec4(0.25f, 0.25f, 1.0f, 1.0f);
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

void Program::savePoints() {
	// Save the control points
	controlPointSave.clear();
	controlPointSave = controlPoints->verts;
	// Save the active point
	activePointSave.clear();
	activePointSave = activePoint->verts;
}


void Program::resetPoints() {
	// Clear points from the screen
	controlPoints->verts.clear();
	renderEngine->updateBuffers(*controlPoints);
	activePoint->verts.clear();
	renderEngine->updateBuffers(*activePoint);
}

void Program::updateControlPoints() {
	// controlPoints->color = glm::vec4(lineColor.x, lineColor.y, lineColor.z, lineColor.w);

	controlPoints->modelMatrix = glm::mat4(1.f);
	controlPoints->modelMatrix = glm::translate(controlPoints->modelMatrix, glm::vec3(translation[0], translation[1], 0.0f));
	controlPoints->modelMatrix = glm::scale(controlPoints->modelMatrix, glm::vec3(scale));
	controlPoints->modelMatrix = glm::rotate(controlPoints->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	renderEngine->updateBuffers(*controlPoints);
}


void Program::addControlPoint(glm::vec3 oldPoint) {
	nurbValues.push_back(1);
	activePointIndex = controlPoints->verts.size();
	updateKnots = true;
	controlPoints->verts.emplace_back(oldPoint);
}

glm::vec4 Program::fixMousePoisiton() const {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::vec4 mousePosFix = glm::vec4(
		((mousePosition->x - (float)width / 2) / ((float)width / 2)) * 10 * (float)width / (float)height,
		(((float)height / 2 - mousePosition->y) / ((float)height / 2)) * 10,
		0.0f, 1.0f
	);
	// std::cout << mousePosFix.x << "," << mousePosFix.y << std::endl;
	return mousePosFix;

}

void Program::addActivePoint() {

	// Boolean for catching just a single click
	mousePosition->z = 0;

	// Convert screen res to screen space
	const glm::vec4 tempMousePosFix = glm::inverse(activePoint->modelMatrix) * fixMousePoisiton();
	const glm::vec3 mousePosFix = glm::vec3(tempMousePosFix.x, tempMousePosFix.y, 0);


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
	const glm::vec4 tempMousePosFix = glm::inverse(activePoint->modelMatrix) * fixMousePoisiton();
	const glm::vec3 mousePosFix = glm::vec3(tempMousePosFix.x, tempMousePosFix.y, 0);
	for (int i = 0; i < controlPoints->verts.size(); i++) {
		if (glm::distance(mousePosFix, controlPoints->verts[i]) < 0.35f) {
			activePoint->verts[0] = controlPoints->verts[i];
			activePointIndex = i;
			return true;
		}
	}
	return false;
}

void Program::moveActivePoint() const
{
	const glm::vec4 tempMousePosFix = glm::inverse(activePoint->modelMatrix) * fixMousePoisiton();
	const glm::vec3 mousePosFix = glm::vec3(tempMousePosFix.x, tempMousePosFix.y, 0);
	activePoint->verts[0] = mousePosFix;
	controlPoints->verts[activePointIndex] = mousePosFix;
	renderEngine->updateBuffers(*controlPoints);
}

void Program::removeActivePoint() {

	if(controlPoints->verts.empty()) {
		return; // If array is already empty, return so we don't crash
	}

	// Otherwise remove the active point and assign a new active point
	controlPoints->verts.erase(controlPoints->verts.begin() + activePointIndex);
	nurbValues.erase(nurbValues.begin() + activeKnotIndex);
	if(controlPoints->verts.empty()) {
		activePoint->verts.clear();
		activePointIndex = 0;
	}
	else {
		activePointIndex = controlPoints->verts.size() - 1;
		activePoint->verts[0] = controlPoints->verts[activePointIndex];
	}
	renderEngine->updateBuffers(*controlPoints);
	updateKnots = true;
}


void Program::updateActivePoint() {
	// controlPoints->color = glm::vec4(lineColor.x, lineColor.y, lineColor.z, lineColor.w);

	activePoint->modelMatrix = glm::mat4(1.f);
	activePoint->modelMatrix = glm::translate(activePoint->modelMatrix, glm::vec3(translation[0], translation[1], 0.0f));
	activePoint->modelMatrix = glm::scale(activePoint->modelMatrix, glm::vec3(scale));
	activePoint->modelMatrix = glm::rotate(activePoint->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	// Select and modify control points
	if(mousePosition->z == 1) {
		mousePosition->z = selectControlPoint() ? 11 : 33;
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

int Program::computeDelta(float &uValue) {
	for (int i = 0; i < controlPointSave.size()+curveOrder; ++i)	
	{
		if(uValue>=1.0f) {
			uValue = 1.0f-0.00001;
		}
		if(uValue>=knots[i] && uValue<knots[i+1]){
			// std::cout << uValue << ":" << i << ":" << knots[i] << std::endl;
			return i;
		}
	}
	return -1;
}

 glm::vec3 Program::deBoorAlg(int delta, float uValue){
 	std::vector<glm::vec3> contributorPoints;
 	// float curveDegree = curveOrder - 1;
 	contributorPoints.reserve(curveOrder);
 	for (int i = 0; i < curveOrder; i++)
 	{
		contributorPoints.push_back(controlPointSave[delta - i]*nurbValues[delta-i]);
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

float Program::deBoorAlgWeightsOnly(int delta, float uValue) {
	std::vector<float> contributorPoints;
	// float curveDegree = curveOrder - 1;
	contributorPoints.reserve(curveOrder);
	for (int i = 0; i < curveOrder; i++)
	{
		contributorPoints.push_back(nurbValues[delta - i]);
	}

	for (int r = curveOrder; r >= 2; r--)
	{
		int i = delta;
		for (int s = 0; s <= r - 2; s++)
		{
			float omega = (uValue - knots[i]) / (knots[i + r - 1] - knots[i]);
			contributorPoints[s] = (omega * contributorPoints[s]) + ((1 - omega)*contributorPoints[s + 1]);
			i--;
		}
	}

	return contributorPoints[0];
}

void Program::createStandardKnots(){
	knots.clear();
	for (int i = 0; i < curveOrder - 1; ++i) {
		knots.push_back(0);
	}
	const float knotSpacing = 1.f / float(controlPointSave.size() - curveOrder + 1);
	for (float i = 0; i <= 1; i += knotSpacing) {
		knots.push_back(i);
	}
	for (int i = 0; i < curveOrder - 1; ++i)
	{
		knots.push_back(1);
	}
}

void Program::createUniformKnots() {
	knots.clear();
	// for (int i = 0; i < curveOrder - 1; ++i) {
	// 	knots.push_back(0);
	// }
	const float knotSpacing = 1.f / float(controlPointSave.size() + curveOrder - 1);
	for (float i = 0; i <= 1; i += knotSpacing) {
		knots.push_back(i);
	}
	// for (int i = 0; i < curveOrder - 1; ++i)
	// {
	// 	knots.push_back(1);
	// }
}

void Program::updateBsplineCurve() {
	// Create b-spline curve
	// checks and preprocessing
	bsplineCurve->color = glm::vec4(lineColor.x, lineColor.y, lineColor.z, lineColor.w);
	bsplineCurve->modelMatrix = glm::mat4(1.f);
	bsplineCurve->modelMatrix = glm::translate(bsplineCurve->modelMatrix, glm::vec3(translation[0], translation[1], 0.0f));
	bsplineCurve->modelMatrix = glm::scale(bsplineCurve->modelMatrix, glm::vec3(scale));
	bsplineCurve->modelMatrix = glm::rotate(bsplineCurve->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	const int uIncTemp = uIncrement;
	const float uOffset = (1.f / (float)uIncTemp);
	bsplineCurve->verts.reserve(uIncTemp);
	// Iterate through u values and generate curve
	float u = knots[0];
	for (int i = 0; i <= uIncTemp; i++)
	{
		// Get the index of the control point that matters
		int delta = computeDelta(u);
		if (delta < 0) { return; }
		// std::cout << "Delta: " << delta << std::endl;
		// Calculate the point on the curve
		bsplineCurve->verts.push_back(deBoorAlg(delta, u)/deBoorAlgWeightsOnly(delta,u));
		u += uOffset;
	}
	renderEngine->updateBuffers(*bsplineCurve);
}

void Program::deBoorAlgShow(int delta) {
	std::vector<glm::vec3> contributorPoints;
	// float curveDegree = curveOrder - 1;
	contributorPoints.reserve(curveOrder);
	for (int i = 0; i < curveOrder; i++)
	{
		contributorPoints.push_back(controlPointSave[delta - i]);
	}

	for (int r = curveOrder; r >= 2; r--)
	{
		int i = delta;
		for (int s = 0; s <= r - 2; s++)
		{
			float omega = (demoU - knots[i]) / (knots[i + r - 1] - knots[i]);
			demoLines->verts.push_back(contributorPoints[s+1]);
			demoLines->verts.push_back(contributorPoints[s]);
			contributorPoints[s] = (omega * contributorPoints[s]) + ((1 - omega)*contributorPoints[s + 1]);
			i--;
		}
	}

	// contributorPoints[0];
}

void Program::updateDemoLines() {
	// create lines to show bspline curve generation graphically/visibly
	demoLines->modelMatrix = glm::mat4(1.f);
	demoLines->modelMatrix = glm::translate(demoLines->modelMatrix, glm::vec3(translation[0], translation[1], 0.0f));
	demoLines->modelMatrix = glm::scale(demoLines->modelMatrix, glm::vec3(scale));
	demoLines->modelMatrix = glm::rotate(demoLines->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	// Get the index of the control point that matters
	int delta = computeDelta(demoU);
	if (delta < 0) { return; }
	deBoorAlgShow(delta);

	renderEngine->updateBuffers(*demoLines);
}

void Program::updateDemoPoint() {
	// draw the current u value as specified in the ui
	demoPoint->verts.clear();
	demoPoint->modelMatrix = glm::mat4(1.f);
	demoPoint->modelMatrix = glm::translate(demoPoint->modelMatrix, glm::vec3(translation[0], translation[1], 0.0f));
	demoPoint->modelMatrix = glm::scale(demoPoint->modelMatrix, glm::vec3(scale));
	demoPoint->modelMatrix = glm::rotate(demoPoint->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	// Iterate through u values and generate curve
	// Get the index of the control point that matters
	int delta = computeDelta(demoU);
	if (delta < 0) { return; }
	// std::cout << "Delta: " << delta << std::endl;
	// Calculate the point on the curve
	demoPoint->verts.push_back(deBoorAlg(delta, demoU)/ deBoorAlgWeightsOnly(delta,demoU));
	renderEngine->updateBuffers(*demoPoint);
}

void Program::createKnots() {
	activeKnot = std::make_shared<Geometry>();
	activeKnot->drawMode = GL_POINTS;
	activeKnot->color = glm::vec4(1.0f, 0.25f, 1.0f, 1.0f);
	renderEngine->assignBuffers(*activeKnot);
	geometryObjects.push_back(activeKnot);

	knotsRender = std::make_shared<Geometry>();
	knotsRender->drawMode = GL_POINTS;
	knotsRender->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	renderEngine->assignBuffers(*knotsRender);
	geometryObjects.push_back(knotsRender);
}

bool Program::selectKnot() {
	// Convert screen res to screen space
	const glm::vec4 tempMousePosFix = fixMousePoisiton();
	const glm::vec3 mousePosFix = glm::vec3(tempMousePosFix.x, tempMousePosFix.y, 0);
	for (int i = curveOrder; i < knotsRender->verts.size()-curveOrder; i++) {
		if (glm::distance(mousePosFix, knotsRender->verts[i]) < 0.35f) {
			if(activeKnot->verts.empty())
			{
				activeKnot->verts.push_back(knotsRender->verts[i]);
			} else {
				activeKnot->verts[0] = knotsRender->verts[i];
			}
			activeKnotIndex = i;
			return true;
		}
	}
	return false;
}

void Program::moveKnot() {
	const glm::vec4 tempMousePosFix = fixMousePoisiton();
	const glm::vec3 mousePosFix = glm::vec3(tempMousePosFix.x, -9, 0);
	float normMousePos = (mousePosFix.x + 12) / 24;
	if (knots[activeKnotIndex - 1] > normMousePos)
	{
		normMousePos = knots[activeKnotIndex - 1]+0.0001;
	}
	if (knots[activeKnotIndex + 1] < normMousePos)
	{
		normMousePos = knots[activeKnotIndex + 1]-0.0001;
	}
	// activeKnot->verts[0] = mousePosFix;
	if (activeKnot->verts.empty())
	{
		activeKnot->verts.emplace_back(normMousePos * 24 - 12 ,-9, 0);
	}
	else {
		activeKnot->verts[0] = glm::vec3(normMousePos * 24 - 12, -9, 0);
	}
	knotsRender->verts[activeKnotIndex] = glm::vec3(normMousePos * 24 - 12, -9, 0);
	knots[activeKnotIndex] = normMousePos;
	renderEngine->updateBuffers(*knotsRender);
}

void Program::updateActiveKnot() {
	knotsRender->verts.clear();
	activeKnot->verts.clear();
	knotsRender->verts.reserve(knots.size());
	for (int i = 0; i < knots.size(); i++)
	{
		knotsRender->verts.emplace_back(knots[i]*24-12, -9, 0);
	}
	// Select and modify control points
	if (mousePosition->z == 33 || mousePosition->z == 1) {
		mousePosition->z = selectKnot() ? 22 : 0;
	}
	if (mousePosition->z == 22) {
		moveKnot();
	}
	renderEngine->updateBuffers(*knotsRender);
	renderEngine->updateBuffers(*activeKnot);
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

		ImGui::ColorEdit3("Clear color", (float*)&clear_color);
		ImGui::ColorEdit4("Line color", (float*)&lineColor);

		ImGui::Text("Transfromations:");
		ImGui::DragFloat("Rotation", (float*)&rotation, 0.1f);
		ImGui::DragFloat("Scale factor", (float*)&scale, 0.001f);
		ImGui::DragFloat2("Translation", (float*)&translation, 0.01f);

		ImGui::Text("Curve parameters:");
		ImGui::DragInt("Order", (int*)&curveOrder, 1, 2, controlPointSave.size());
		ImGui::DragInt("Resolution", (int*)&uIncrement, 1, 1, 10000);
		ImGui::DragFloat("Demo point", (float*)&demoU, 0.001, 0,1);
		
		if(ImGui::Button("Remove point")) {
			removePoint = true;
			// Fix the curve order to match how many control points are left
			if(curveOrder > controlPointSave.size()-1 && curveOrder>2)
			{
				curveOrder = controlPointSave.size()-1;
			}
		}

		// ImGui::Text("Show:");
  //
		// ImGui::SameLine();
		ImGui::Checkbox("Curve", (bool*)&drawCurve);

		ImGui::SameLine();
		ImGui::Checkbox("Control points", (bool*)&drawPoints);

		ImGui::SameLine();
		ImGui::Checkbox("Geometry", (bool*)&drawDemoGeom);
		
		ImGui::SameLine();
		ImGui::Checkbox("Point", (bool*)&drawDemoPoint);

		ImGui::Text("Bonus options:");
		if (ImGui::Button("Use standard knots")) {
			standardKnots = true;
			updateKnots = true;
			uniformKnots = false;
		}

		ImGui::SameLine();
		ImGui::Checkbox("Draw knots", (bool*)&drawKnots);

		// ImGui::SameLine();
		// if (ImGui::Button("Use uniform knots")) {
		// 	uniformKnots = true;
		// 	updateKnots = true;
		// 	standardKnots = false;
		// }
		ImGui::DragFloat("NURB Value", (float*)&nurbValues[activePointIndex], 0.001, 0);

		ImGui::End();
	}
}

float oldOrder = 0;
// Main loop
void Program::mainLoop() {
	
	// Our state
	show_test_window = false;
	clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	lineColor = ImVec4(1.0f, 1.0f, 0.0f, 1.00f);

	// createTestGeometryObject();
	createDemoPoint();
	createKnots();
	createActivePoint();
	createControlPoints();
	createBsplineCurve();
	createDemoLines();


	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		resetPoints();
		if(drawPoints) {
			controlPoints->verts = controlPointSave;
			activePoint->verts = activePointSave;
			updateActivePoint();
			updateControlPoints();
			savePoints();
		}

		clearKnots();
		if(drawKnots && !knots.empty())
		{
			updateActiveKnot();
		}

		clearCurve();
		if (controlPointSave.size() >= curveOrder && drawCurve) {
			if(updateKnots || oldOrder!= curveOrder)
			{
				updateKnots = false;
				oldOrder = curveOrder;
				if (standardKnots) {
					createStandardKnots();
					// standardKnots = false;
				}
				// if(uniformKnots)
				// {
				// 	createUniformKnots();
				// 	// uniformKnots = false;
				// }
				// std::cout << "Knots: ";
				// for (float knot : knots)
				// {
				// 	std::cout << knot << ",";
				// }
				// std::cout << std::endl;
			}

			updateBsplineCurve();
			if(drawDemoGeom) {
				updateDemoLines();
			}
			if(drawDemoPoint) {
				updateDemoPoint();
			}
		}
		
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
