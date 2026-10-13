#pragma once

#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Render/Meshes/TileMesh.hpp>
#include <Robot/Render/Meshes/AgentMesh.hpp>

class PlaygroundView
{
public:
	PlaygroundView(PlaygroundViewListener * listener);

	void init();
	void update();
	void draw();

private:
	std::vector<TileMesh> map_;
	AgentMesh agent_;

	bool itemStashFocus_ = false;

private:
	PlaygroundViewListener * listener_;
};