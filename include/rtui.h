//
// Created by 何振邦 on 2019-06-09.
//

#ifndef HJRT_RTUI_H
#define HJRT_RTUI_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "film.h"
#include "pathtracer.h"
class RTUI {
protected:
	GLFWwindow *windowPtr;
	std::string windowTitle;
	int width;
	int height;
	int bufferWidth;
	int bufferHeight;

	std::chrono::high_resolution_clock::time_point lasttime;

	virtual void framebufferSizeCallback(GLFWwindow *window, int width, int height);

	virtual void inputCallback();

	virtual void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

	virtual void mouseCallback(GLFWwindow *window, double xpos, double ypos);

	virtual void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

	virtual void render();

	virtual void customInit();

	static RTUI *currentWindow;
	static bool isFirstInit;

public:

	Film* film;
	PathTracer* pathTracer;
	GLuint texID;

	RTUI();

	~RTUI();

	RTUI(int _width, int _height, std::string _title);


	//enquiring functions
	int getWidth() const { return width; }

	int getHeight() const { return height; }

	GLFWwindow *getWindowPtr() const { return windowPtr; }

	bool shouldClose() const;

	//setting functions
	void setWidth(int _width);

	void setHeight(int _height);

	void setWindowPosition(int x, int y);

	void setCurrentContext();

	void swapBuffer();

	virtual void run();

	static void InitGLFWEnvironment(int versionMajor = 3, int versionMinor = 3, int profile = GLFW_OPENGL_CORE_PROFILE,int COMPAT = GL_TRUE);

	static void staticMouseCallback(GLFWwindow *window, double xpos, double ypos);

	static void staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

	static void staticFramebufferSizeCallback(GLFWwindow *window, int width, int height);

	static void staticScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
};


#endif //HJRT_RTUI_H
