#include "Program.h"

// float Program::offset[] = { 0,0 };

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

	/*
	bool err = gl3wInit() != 0;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
	}
	*/
	renderEngine = new RenderEngine(window);

	mousePosition = new glm::vec3(0);

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
	Geometry* testObject = new Geometry();

	testObject->verts.push_back(glm::vec3(-5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(0.f, 5.f, 0.f));
	renderEngine->assignBuffers(*testObject);
	renderEngine->updateBuffers(*testObject);
	geometryObjects.push_back(testObject);
}

void Program::createControlPoints() {
	controlPoints = new Geometry();
	controlPoints->drawMode = GL_POINTS;

	renderEngine->assignBuffers(*controlPoints);
	// updateControlPoints();
	// renderEngine->updateBuffers(*controlPoints);
	geometryObjects.push_back(controlPoints);

}

void Program::createActivePoint() {
	activePoint = new Geometry();
	activePoint->drawMode = GL_POINTS;
	activePoint->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	renderEngine->assignBuffers(*activePoint);
	// updateActivePoint();
	// renderEngine->updateBuffers(*controlPoints);
	geometryObjects.push_back(activePoint);
}

void Program::createBsplineCurve() {
	bsplineCurve = new Geometry();

}

void Program::createDemoLines() {
	demoLines = new Geometry();

}

void Program::createDemoPoint() {
	demoPoint = new Geometry();

}

void Program::updateControlPoints() {

}


void Program::addControlPoint(glm::vec3 oldPoint) {
	 controlPoints->color = glm::vec4(lineColor.x, lineColor.y, lineColor.z, lineColor.w);

    controlPoints->modelMatrix = glm::mat4(1.f);
    controlPoints->modelMatrix = glm::scale(controlPoints->modelMatrix, glm::vec3(scale));
    controlPoints->modelMatrix = glm::rotate(controlPoints->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	controlPoints->verts.emplace_back(oldPoint);


    renderEngine->updateBuffers(*controlPoints);
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

void Program::selectControlPoint() {
	// Convert screen res to screen space
	const glm::vec3 mousePosFix = fixMousePoisiton();
	for each (glm::vec3 point in controlPoints->verts) {
		if(glm::distance(mousePosFix,point)<0.35f) {
			activePoint->verts[0] = point;
		}
	}
}

void Program::updateActivePoint() {
	// controlPoints->color = glm::vec4(lineColor.x, lineColor.y, lineColor.z, lineColor.w);

	activePoint->modelMatrix = glm::mat4(1.f);
	activePoint->modelMatrix = glm::scale(activePoint->modelMatrix, glm::vec3(scale));
	activePoint->modelMatrix = glm::rotate(activePoint->modelMatrix, glm::radians(rotation), glm::vec3(0, 0, 1.0f));

	if (mousePosition->z == 1) {
		selectControlPoint();
		mousePosition->z = 0;
	}

	if (mousePosition->z == 2) {
		addActivePoint();
		mousePosition->z = 0;
	}



	renderEngine->updateBuffers(*activePoint);
}

void Program::updateBsplineCurve() {

}

void Program::updateDemoLines() {

}

void Program::updateDemoPoint() {

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

		ImGui::DragInt("B-spline order (k value)", (int*)&curveOrder, 1);
		if (curveOrder < 1) {
			curveOrder = 1;
		}
		ImGui::DragInt("B-spline resolution (u increment)", (int*)&uIncrement, 1);
		if (uIncrement < 1) {
			uIncrement = 1;
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

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		updateActivePoint();
		updateControlPoints();

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
