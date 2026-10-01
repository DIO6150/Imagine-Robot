#pragma once

#include <string>

class IAction
{
	virtual ~IAction() = 0;

	virtual std::string getName() = 0;
};