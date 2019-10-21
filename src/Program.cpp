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
		// ImGui::DragInt("number of cycles", (int*)&cycles);
		// if (cycles < 1) {
		// 	cycles = 1;
		// }
		
		ImGui::DragFloat("rotation", (float*)&rotation, 0.1f);
		ImGui::DragFloat("scale factor", (float*)&scale, 0.001f);
		ImGui::DragInt("B-spline resolution", (int*)&step, 1);
		if (step < 1) {
			step = 1;
		}
		// ImGui::DragInt("draws before updating", (int*)&amount, 1);
		// if(amount<0) {
		// 	amount = 0;
		// }

		// if (ImGui::Button("refresh")) {
		// 	parametersChanged = true;
		// 	theta = 0;
		// 	thetaCi = 0;
		// 	thetaCo = 0;
		// 	enablePoints = false;
		// }

		// ImGui::SameLine();
		//
		// if (ImGui::Button("reset to defaults")) {
		// 	outerRadius = 4;
		// 	innerRadius = 1;
		// 	cycles = 1;
		// 	rotation = 0;
		// 	scale = 1;
		// 	amount = 1;
		// 	step = 100;
		// 	circleDetail = 100;
		// 	parametersChanged = true;
		// 	hideInnerCircle = false;
		// 	hideOuterCircle = false;
		// 	hideDot = false;
		// 	enablePoints = false;
		// 	offset[0] = 0;
		// 	offset[1] = 0;
		//
		//
		// 	theta = 0;
		// 	thetaCi = 0;
		// 	thetaCo = 0;
		// }
		//
		// ImGui::SameLine();
		//
		// ImGui::Checkbox("pause animation", (bool*)&pauseAnimation);
		//
		// ImGui::SameLine();
		//
		// ImGui::Checkbox("view hypocycloid", (bool*)&viewHypocycloid);
		//
		// ImGui::DragInt("circle resolutions", (int*)&circleDetail, 1);
		// if (circleDetail < 1) {
		// 	circleDetail = 1;
		// }
		// ImGui::Checkbox("hide inner circle", (bool*)&hideInnerCircle);
		//
		// ImGui::SameLine();
		//
		// ImGui::Checkbox("hide outer circle", (bool*)&hideOuterCircle);
		//
		// ImGui::SameLine();
		//
		// ImGui::Checkbox("hide leading dot", (bool*)&hideDot);
		//
		//
		//
		// if (ImGui::Button("apply scale & translation to points")) {
		// 	applyPolynomialScale = true;
		// }
		//
		// ImGui::SameLine();
		//
		// ImGui::Checkbox("enable placing control points", (bool*)&enablePoints);
		// if(enablePoints) {
		// 	mousePosition->z = 0;
		// }
		//
		// ImGui::DragFloat("polynomial point scale", (float*)&polynomialScale, 0.001f);
		//
		// ImGui::DragFloat2("translate the model", (float*)&offset, 0.01f);

		ImGui::End();
	}
}

// Main loop
void Program::mainLoop() {
	// createTestGeometryObject();
	// createCycloid();
	//
	// createOuterCircle();
	// createInnerCircle();
	// createLastPoint();
	//
	// createPolynomial();

	createTestGeometryObject();
	
	// Our state
	show_test_window = false;
	clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	lineColor = ImVec4(1.0f, 1.0f, 0.0f, 1.00f);

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		
		drawUI();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		renderEngine->render(geometryObjects, glm::mat4(1.f), glm::vec4(lineColor.x,lineColor.y,lineColor.z,lineColor.w));
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
