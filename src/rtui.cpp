//
// Created by 何振邦 on 2019-06-09.
//

#include "rtui.h"

RTUI *RTUI::currentWindow = nullptr;
bool RTUI::isFirstInit = true;

RTUI::RTUI(int _width, int _height, std::string _title) : width(_width), height(_height), windowTitle(_title) {
	if (isFirstInit) {
		isFirstInit = false;
		InitGLFWEnvironment();
	}

	windowPtr = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
	if (windowPtr == nullptr) {
		std::cout << "ERROR @ GLFWWindows : Can't create window " << windowTitle << std::endl;
		throw "ERROR @ GLFWWindows : failed to create glfw window";
	}
	glfwSetFramebufferSizeCallback(windowPtr, staticFramebufferSizeCallback);
	glfwSetCursorPosCallback(windowPtr, staticMouseCallback);
	glfwSetScrollCallback(windowPtr, staticScrollCallback);
	glfwSetMouseButtonCallback(windowPtr, staticMouseButtonCallback);
	glfwGetFramebufferSize(windowPtr, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(windowPtr);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	currentWindow = this;
}

RTUI::RTUI() : RTUI(800, 600, "Window") {

}

RTUI::~RTUI() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(windowPtr);
}

void RTUI::run() {
	glfwMakeContextCurrent(windowPtr);
	customInit();
	while (!shouldClose()) {
//		inputCallback();
		render();
		swapBuffer();
		glfwPollEvents();
	}
}

/*
 * This function is called when we need to render something
 */
void RTUI::render() {
	static bool lastFrameFinished = false;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImVec4 clear_color = ImVec4(0.333, 0.341, 0.345, 1.00);
	{
		auto current = std::chrono::high_resolution_clock::now();
		auto secDura = std::chrono::duration_cast<std::chrono::milliseconds>(current - lasttime);
		//update rendering result
		bool finish = pathTracer->isFinish();
		if (secDura.count() > 250) {
			lasttime = current;
			if (!finish || (!lastFrameFinished)) {
				if (finish)lastFrameFinished = true;
				glBindTexture(GL_TEXTURE_2D, texID);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, film->width, film->height, 0, GL_RGB,
				             GL_UNSIGNED_BYTE, film->getDiscretizedData());
			}
		}

		ImGui::Begin("Rendering result", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(film->width + 40, film->height + 40));
		ImGui::SetCursorPos(ImVec2(20, 20));
		ImGui::Image((void *) texID, ImVec2(film->width, film->height));
		ImGui::End();

		ImGui::Begin("Rendering Info");
		ImGui::Text("[Parameter]");
		ImGui::Text("size: %dx%d", film->width, film->height);
		int samples = pathTracer->samplingTex.getUniformSamples();
		if (samples > 1)
			ImGui::Text("samples: %d", samples);
		else
			ImGui::Text("samples: adaptive");
		ImGui::Text("anti-alias: %d", pathTracer->antiAliasNum);
		ImGui::Text("[Progress ]");
		for (int i = 0; i < pathTracer->runningThreadNum; ++i) {
			ImGui::Text("thread %d", i);
			ImGui::Text(" -block progress: %.1f%%", pathTracer->blockProgress[i] * 100);
		}
		ImGui::ProgressBar(pathTracer->totalProgress());
		if (finish)ImGui::Text("total time: %ds", pathTracer->latestRenderSec);

		ImGui::End();
	}
	// Rendering
	ImGui::Render();

	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/*
 * This function is called before render() is called for the first time
 */
void RTUI::customInit() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void) io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(windowPtr, true);
	const char *glsl_version = "#version 150";
	ImGui_ImplOpenGL3_Init(glsl_version);
	if (film != nullptr) {
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, film->width, film->height, 0, GL_RGB,
		             GL_UNSIGNED_BYTE, film->getDiscretizedData());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	lasttime = std::chrono::high_resolution_clock::now();
}


void RTUI::setHeight(int _height) {
	height = _height;
	glfwSetWindowSize(windowPtr, width, height);
}

void RTUI::setWidth(int _width) {
	width = _width;
	glfwSetWindowSize(windowPtr, width, height);
}

void RTUI::setWindowPosition(int x, int y) {
	glfwSetWindowPos(windowPtr, x, y);
}

bool RTUI::shouldClose() const {
	if (pathTracer->state == Integrator::RENDERING)return false;
	return glfwWindowShouldClose(windowPtr);
}

void RTUI::InitGLFWEnvironment(int versionMajor, int versionMinor, int profile, int compat) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, compat);
}

void RTUI::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void RTUI::inputCallback() {

}

void RTUI::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

}

void RTUI::mouseCallback(GLFWwindow *window, double xpos, double ypos) {

}

void RTUI::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {

}

void RTUI::swapBuffer() {
	glfwSwapBuffers(windowPtr);
}

void RTUI::setCurrentContext() {
	glfwMakeContextCurrent(windowPtr);
}

void RTUI::staticFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
	if (currentWindow == nullptr)return;
	currentWindow->framebufferSizeCallback(window, width, height);
}

void RTUI::staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (currentWindow == nullptr)return;
	currentWindow->mouseButtonCallback(window, button, action, mods);
}

void RTUI::staticMouseCallback(GLFWwindow *window, double xpos, double ypos) {
	if (currentWindow == nullptr)return;
	currentWindow->mouseCallback(window, xpos, ypos);
}

void RTUI::staticScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if (currentWindow == nullptr)return;
	currentWindow->scrollCallback(window, xoffset, yoffset);
}