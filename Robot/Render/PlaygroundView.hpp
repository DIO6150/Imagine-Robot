#pragma once

#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Utils/Animation.hpp>

#include <Robot/Render/MapRenderer.hpp>
#include <Robot/Render/ItemRenderer.hpp>

#include <Robot/Simulation/Map.hpp>
#include <Robot/Agent/AgentController.hpp>

#include <glfw/glfw3.h>

class PlaygroundView
{
public:
	PlaygroundView(PlaygroundViewListener * listener);

	void init(Map const & map, AgentController const * agent, GLFWwindow * window);
	void update(double deltaTime);
	void draw();

	Animation moveAgent(Pos2D oldPos, Pos2D newPos);
	Animation equipItem(Item item);
	Animation unequipItem();

private:
	glm::vec2 convertPos(Pos2D tilePos);

private:
	std::optional<ItemInstance> equipedItem_;
	bool itemStashFocus_ = false;

private:
	float simulationSpeed_ = 1.0f;
	double deltaTime_;

	AgentController const * agent_;
	Map const * map_;

	SharedResources resources_;
	MapRenderer mapRenderer_;
	ItemRenderer itemRenderer_;

	std::map<uint32_t, int> keyStates_;

private:
	PlaygroundViewListener * listener_;
	GLFWwindow * window_;
};