#pragma once

#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/Playground.hpp>

#include <Robot/Errors/Error.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Simulation
{
public:
	JSONParserStatus executeDirective(Directive instructions, DirectiveTrace & trace);

private:
	double targetFPS_       = 144.0;
	double targetFrameTime_ = 1.0 / targetFPS_;

	double lastRenderTime_  = 0.0;
	double lastTime_        = 0.0;
	double currentTime_     = 0.0;
	double deltaTime_       = 0.0;

	Playground playground_;

	GLFWwindow * window_;
};


