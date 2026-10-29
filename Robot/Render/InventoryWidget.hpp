#pragma once

#include <Robot/Common/Emotion.hpp>

#include <Robot/Render/SharedResourcesArray.hpp>

#include <map>
#include <string>

#include <glad/glad.h>

#include <Robot/Render/SpriteRenderer.hpp>
#include <Robot/Render/Widget.hpp>

#include <Robot/Simulation/ItemStash.hpp>

#include <glm/gtc/matrix_transform.hpp>

class InventoryWidget: public AbstractWidget
{
public:
	InventoryWidget(SpriteRenderer & renderer, SharedResourceArray & resources);

	void initialize(ItemStash stash, uint32_t windowWidth, uint32_t windowHeight);
	void updateInventory(ItemStash stash);

	std::optional<Sprite> createItem(Item item);

	void setPosition(glm::vec3 position);

	void setCursorPosition(glm::vec3 position);
	void setCursorScale(glm::vec3 scale);
	void hideCursor();
	void showCursor();

	glm::vec3 getOpenPosition();
	glm::vec3 getClosePosition();
	glm::vec3 getItemPosition(Item const & item);
	glm::vec3 getItemRelativePosition(Item const & item);
	glm::vec3 getItemRelativePosition(Pos2D pos);
	glm::vec3 getItemScale();

	void onRender() override;

	NeededResources onResourcePreLoad() override;

private:
	struct PopupWindow
	{
		float margin      = 10.f;
		float itemScale   = 64.f;
		float itemPadding = 10.f;

		Sprite sprite;

		void calculateSize(std::vector<Item> const & inventory);
		glm::vec3 getItemPosition(Item item);
		glm::vec3 getItemPosition(Pos2D pos);
		glm::vec3 getItemScale();
	};

private:
	uint32_t windowWidth_;
	uint32_t windowHeight_;

	std::vector<Sprite> items_;
	PopupWindow popup_;
	Sprite cursor_;
	bool cursorVisible_ = false;
};