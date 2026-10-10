#pragma once

#include <Robot/Agent/AgentController.hpp>

#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Simulation/Map.hpp>
#include <Robot/Simulation/Script.hpp>

#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Render/PlaygroundView.hpp>

class Playground : public PlaygroundViewListener
{
public:
	Playground();

	// Presenter Code
	void start(Directive const & instructions);
	void tick();
	void draw();

private:
	Script script_;
	Map map_;
	ItemStash closet_;
	Dictionary dictionary_;

private:
	std::unique_ptr<AgentController> agent_;
	std::unique_ptr<PlaygroundView> view_;
};