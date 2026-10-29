#pragma once

#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Utils/Animation.hpp>

#include <Robot/Render/MapWidget.hpp>
#include <Robot/Render/SpriteRenderer.hpp>
#include <Robot/Render/InventoryWidget.hpp>

#include <Robot/Simulation/Map.hpp>
#include <Robot/Agent/AgentController.hpp>

#include <glfw/glfw3.h>

class PlaygroundView
{
public:
	PlaygroundView(PlaygroundViewListener * listener);

	void init(Map const & map, AgentController const & agent, ItemStash const & stash, GLFWwindow * window);
	void update(double deltaTime);
	void draw();

	Animation moveAgent(Pos2D oldPos, Pos2D newPos);
	Animation equipItem(Item item, ItemStash stash);
	Animation giveItem(Pos2D resident);
	Animation openChest();
	Animation closeChest();
	Animation moveChestCursor(Pos2D old, Pos2D cursor);

private:
	glm::vec2 convertPos(Pos2D tilePos);

private:
	std::optional<Sprite> equipedItem_;
	bool carryItem_ = false;

private:
	float simulationSpeed_ = 1.0f;
	double deltaTime_;

	SharedResourceArray resourceArray_;
	SpriteRenderer spriteRenderer_ {resourceArray_};

	MapWidget       mapWidget_       {spriteRenderer_, resourceArray_};
	InventoryWidget inventoryWidget_ {spriteRenderer_, resourceArray_};


	std::map<uint32_t, int> keyStates_;

private:
	PlaygroundViewListener * listener_;
	GLFWwindow * window_;
};