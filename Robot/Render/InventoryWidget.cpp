
#include <Robot/Render/InventoryWidget.hpp>

#include <Robot/Render/Image.hpp>

InventoryWidget::InventoryWidget(SpriteRenderer & renderer, SharedResourceArray & resources)
	: AbstractWidget {renderer, resources}
{
	// "popup"
}

void InventoryWidget::initialize(ItemStash stash, uint32_t windowWidth, uint32_t windowHeight)
{
	windowWidth_  = windowWidth;
	windowHeight_ = windowHeight;
	
	popup_.sprite = renderer_.createSprite("default", "default", "inventory").value();
	//popup_.sprite.color   = {.81, .77, .72};

	cursor_ = renderer_.createSprite("default", "default", "cursor").value();
	cursor_.scale = getItemScale();
	cursor_.position.z = .5f;
	
	updateInventory(stash);
}

void InventoryWidget::updateInventory(ItemStash stash)
{
	auto inventory = stash.inventory();

	items_.clear();
	popup_.calculateSize(inventory);

	for (auto const & item : inventory)
	{
		auto itemSprite = createItem(item);
		if (!itemSprite) continue;
		items_.emplace_back(itemSprite.value());
	}
}

std::optional<Sprite> InventoryWidget::createItem(Item item)
{
	static const std::map<Emotion, std::string> EMOTION = {
		{Emotion::Anger,        "emotion/anger"       },
		{Emotion::Anticipation, "emotion/anticipation"},
		{Emotion::Confidence,   "emotion/confidence"  },
		{Emotion::Disgust,      "emotion/disgust"     },
		{Emotion::Fear,         "emotion/fear"        },
		{Emotion::Joy,          "emotion/joy"         },
		{Emotion::Sadness,      "emotion/sadness"     },
		{Emotion::Surprised,    "emotion/surprised"   },
	};

	static const std::map<Intensity, glm::vec3> INTENSITY = {
		{Intensity::Small , glm::vec3 { 1.0, 0.0, 0.0 } },
		{Intensity::Mid   , glm::vec3 { 0.0, 1.0, 0.0 } },
		{Intensity::Strong, glm::vec3 { 0.0, 0.0, 1.0 } },
	};

	if (item.name == "vide") return std::nullopt;

	auto result = renderer_.createSprite("default", "default");
	if (!result) return std::nullopt;

	auto itemSprite = result.value();

	itemSprite.texture = getTexture(EMOTION.at(item.emotion));
	itemSprite.color   = INTENSITY.at(item.intensity);
	itemSprite.position = popup_.getItemPosition(item);
	itemSprite.scale    = popup_.getItemScale();
	itemSprite.updateTransform();

	return itemSprite;
}

void InventoryWidget::setPosition(glm::vec3 position)
{
	popup_.sprite.position = position;
	popup_.sprite.updateTransform();
}

void InventoryWidget::setCursorPosition(glm::vec3 position)
{
	cursor_.position = position;
}

void InventoryWidget::setCursorScale(glm::vec3 scale)
{
	cursor_.scale = scale;
}

void InventoryWidget::showCursor()
{
	cursorVisible_ = true;
}

void InventoryWidget::hideCursor()
{
	cursorVisible_ = false;
}


glm::vec3 InventoryWidget::getOpenPosition()
{
	return {
		(windowWidth_ - popup_.sprite.scale.x) * 0.5f,
		10.0f,
		0.0f
	};
}

glm::vec3 InventoryWidget::getClosePosition()
{
	return {
		(windowWidth_ - popup_.sprite.scale.x) * 0.5f,
		- popup_.sprite.scale.y,
		0.0f
	};
}

glm::vec3 InventoryWidget::getItemPosition(Item const & item)
{
	return popup_.getItemPosition(item) + popup_.sprite.position;
}

glm::vec3 InventoryWidget::getItemRelativePosition(Item const & item)
{
	return popup_.getItemPosition(item);
}

glm::vec3 InventoryWidget::getItemRelativePosition(Pos2D pos)
{
	return popup_.getItemPosition(pos);
}

glm::vec3 InventoryWidget::getItemScale()
{
	return {popup_.itemScale, popup_.itemScale, 1.0f};
}

void InventoryWidget::onRender()
{
	renderer_.push(popup_.sprite);

	for (auto item : items_)
	{
		item.position += popup_.sprite.position;
		item.position.z = 0.01f;
		item.updateTransform();

		renderer_.push(item);
	}

	if (cursorVisible_)
	{
		Sprite cursor = cursor_;
		cursor.position += popup_.sprite.position;
		cursor.updateTransform();
		renderer_.push(cursor);
	}
}

NeededResources InventoryWidget::onResourcePreLoad()
{
	NeededResources needed;
	needed.meshes = {

	};

	needed.textures = {
		"inventory",
		"cursor",
		"emotion/anger",
		"emotion/anticipation",
		"emotion/confidence",
		"emotion/disgust",
		"emotion/fear",
		"emotion/joy",
		"emotion/sadness",
		"emotion/surprised",
	};

	needed.shaders = {
		
	};

	return needed;
}

void InventoryWidget::PopupWindow::calculateSize(std::vector<Item> const & inventory)
{
	sprite.scale = {.0f, .0f, 1.};
	for (auto const & item : inventory)
	{
		sprite.scale.x = std::max(sprite.scale.x, (itemScale + itemPadding) * item.pos.c);
		sprite.scale.y = std::max(sprite.scale.y, (itemScale + itemPadding) * item.pos.l);
	}

	sprite.scale += itemScale + margin * 2.f;
	sprite.scale.z = 1.0f;

	sprite.updateTransform();
}

glm::vec3 InventoryWidget::PopupWindow::getItemPosition(Item item)
{
	return getItemPosition(item.pos);
}

glm::vec3 InventoryWidget::PopupWindow::getItemPosition(Pos2D pos)
{
	return {
		margin + pos.c * (itemScale + itemPadding),
		margin + (2 - pos.l) * (itemScale + itemPadding),
		0.01f
	};
}

glm::vec3 InventoryWidget::PopupWindow::getItemScale()
{
	return { itemScale, itemScale, 1.0};
}