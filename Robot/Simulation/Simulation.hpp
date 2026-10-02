#pragma once

#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/Playground.hpp>

#include <Robot/Errors/Error.hpp>

class Simulation
{
public:
	DirectiveTrace executeDirective(Directive instructions);

private:
	Playground playground_;
};

