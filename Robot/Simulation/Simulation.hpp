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
	Playground playground_;

	GLFWwindow * window_;
};


