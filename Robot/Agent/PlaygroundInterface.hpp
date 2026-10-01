#pragma once

#include <memory>

#include <Robot/Errors/Error.hpp>

#include <Robot/Simulation/Directive.hpp>

class Playground;

class PlaygroundInterface
{
public:
	Error moveUp();
	Error moveDown();
	Error moveLeft();
	Error moveRight();

private:
	Playground * base_;
	DirectiveTrace * trace_;

};
