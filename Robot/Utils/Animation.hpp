#pragma once

#include <functional>

enum AsyncState
{
	Stop,
	Yield
};

using Animation = std::function<AsyncState()>;