#pragma once

#include <fstream>

#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/Playground.hpp>

#include <Robot/Errors/Error.hpp>

class Simulation
{
public:
	DirectiveTrace executeDirective(Directive instructions);

private:

private:
	Playground playground_;
};

