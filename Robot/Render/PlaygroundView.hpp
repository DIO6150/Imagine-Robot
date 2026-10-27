#pragma once

#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Render/MapRenderer.hpp>

#include <Robot/Simulation/Map.hpp>

class PlaygroundView
{
public:
	PlaygroundView(PlaygroundViewListener * listener);

	void init(Map const & map);
	void update();
	void draw();

private:
	MapRenderer physicalMap_;

	bool itemStashFocus_ = false;

private:
	PlaygroundViewListener * listener_;
};