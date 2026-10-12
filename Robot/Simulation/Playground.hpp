#pragma once

#include <iostream>

#include <Robot/Agent/AgentController.hpp>

#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Simulation/Map.hpp>
#include <Robot/Simulation/Script.hpp>

#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Render/PlaygroundView.hpp>

class AgentController;

class Playground : public PlaygroundViewListener
{
public:
	Playground();
	
	// Presenter Code
	void start(Directive const & instructions);
	void tick();
	void draw();
	
	bool agentIsOver();
	bool agentCanMove();
	std::vector<Tile> agentSee();
	std::tuple<Emotion, Intensity> agentConsult(std::string message);
	int agentSearch(Pos2D posDrawer);
	void agentTake(Pos2D posDrawer, std::string itemName);
	
private:
	JSONParserStatus parse(std::string const & path, std::string toFill);

private:
	Script script_;
	Map map_;
	ItemStash closet_;
	Dictionary dictionary_;

private:
	std::unique_ptr<AgentController> agent_;
	std::unique_ptr<PlaygroundView> view_;
};
