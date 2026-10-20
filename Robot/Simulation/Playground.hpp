#pragma once

#include <iostream>

#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Simulation/Map.hpp>
#include <Robot/Simulation/Script.hpp>

#include <Robot/Common/PlaygroundAgentListener.hpp>
#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Render/PlaygroundView.hpp>

#include <Robot/Agent/AgentController.hpp>

#include <Robot/Utils/Animation.hpp>

class GLFWwindow;

class Playground : public PlaygroundViewListener, public PlaygroundAgentListener
{
public:
	Playground();
	
	// Presenter Code
	void start(GLFWwindow * window, Directive const & instructions);
	void tick();
	void draw();

public:
	void onMapInit(std::vector<Tile> map, MapInfo info) override;

	void onAgentMove(Pos2D newPos) override;

	void onAgentTakeItem(Item item) override;
	void onAgentGiveItem(Item item) override;

	void onAgentStashOpen () override;
	void onAgentStashClose() override;

	void onAgentConsult() override;

	void onAgentSelect(Pos2D cursor) override;

	bool agentIsOver();
	
private:

	bool canMove() override;
	std::vector<Tile> see() override;
	std::tuple<Emotion, Intensity> consult(std::string message) override;
	std::string search(Pos2D posDrawer) override;
	bool take(Pos2D posDrawer, std::string itemName) override;
	
private:
	JSONParserStatus parse(std::string const & path, std::string toFill);

private:
	Script script_;
	Map map_;
	ItemStash closet_;
	Dictionary dictionary_;

	GLFWwindow * window_;
	std::vector<Animation> animationStack_;

private:
	std::unique_ptr<AgentController> agent_;
	std::unique_ptr<PlaygroundView> view_;
};
