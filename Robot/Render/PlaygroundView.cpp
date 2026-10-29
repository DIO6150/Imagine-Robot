
#include <Robot/Render/PlaygroundView.hpp>

#include <Robot/Utils/Easing.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <Robot/Render/Image.hpp>
#include <stb/stb_image.h>

PlaygroundView::PlaygroundView(PlaygroundViewListener * listener)
	: listener_ {listener}
{
	
}

void PlaygroundView::init(
	Map const & map,
	AgentController const & agent,
	ItemStash const & stash,
	GLFWwindow * window
)
{
	window_ = window;

	spriteRenderer_.initializeDefaults();

	int width, height;
	glfwGetWindowSize(window_, &width, &height);
	spriteRenderer_.setProjectionMatrix(glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height)));

	Image blank, empty, wall, chest, chest_open, guy, guy_carry, dictionary,
	anger, anticipation, confidence, disgust, fear, joy, sadness, surprised,
	inventory, cursor;
	stbi_set_flip_vertically_on_load(true);

	empty       .load("data/assets/textures/empty.png");
	wall        .load("data/assets/textures/wall.png");
	chest       .load("data/assets/textures/chest.png");
	chest_open  .load("data/assets/textures/chest_open.png");
	guy         .load("data/assets/textures/guy.png");
	guy_carry   .load("data/assets/textures/guy_carrying.png");
	dictionary  .load("data/assets/textures/dictionnary.png");
	anger       .load("data/assets/textures/emotion_anger.png");
	anticipation.load("data/assets/textures/emotion_anticipation.png");
	confidence  .load("data/assets/textures/emotion_confidence.png");
	disgust     .load("data/assets/textures/emotion_disgust.png");
	fear        .load("data/assets/textures/emotion_fear.png");
	joy         .load("data/assets/textures/emotion_joy.png");
	sadness     .load("data/assets/textures/emotion_sadness.png");
	surprised   .load("data/assets/textures/emotion_surprised.png");
	cursor      .load("data/assets/textures/selector.png");
	inventory   .load("data/assets/textures/inventory.png");

	resourceArray_.createTexture("empty"               , empty     );
	resourceArray_.createTexture("wall"                , wall      );
	resourceArray_.createTexture("chest"               , chest     );
	resourceArray_.createTexture("chest_open"          , chest_open);
	resourceArray_.createTexture("guy"                 , guy       );
	resourceArray_.createTexture("guy_carry"           , guy_carry );
	resourceArray_.createTexture("dictionary"          , dictionary);
	resourceArray_.createTexture("emotion/anger"       , anger);
	resourceArray_.createTexture("emotion/anticipation", anticipation);
	resourceArray_.createTexture("emotion/confidence"  , confidence);
	resourceArray_.createTexture("emotion/disgust"     , disgust);
	resourceArray_.createTexture("emotion/fear"        , fear);
	resourceArray_.createTexture("emotion/joy"         , joy);
	resourceArray_.createTexture("emotion/sadness"     , sadness);
	resourceArray_.createTexture("emotion/surprised"   , surprised);
	resourceArray_.createTexture("cursor"              , cursor);
	resourceArray_.createTexture("inventory"           , inventory);

	if (mapWidget_.canInitialize())
		mapWidget_.initialize(map.rawTileMap(), map.getWidth(), map.getHeight(), agent.getPosition(), width, height);
	else std::cout << "ERROR CANT DO SHIT IN DETROIT MAP WIDGET NOT LOADED\n";

	if (inventoryWidget_.canInitialize())
	{
		inventoryWidget_.initialize(stash, width, height);
		inventoryWidget_.setPosition(inventoryWidget_.getClosePosition());
	}
	else std::cout << "ERROR CANT DO SHIT IN DETROIT INVENTORY WIDGET NOT LOADED\n";
}

void PlaygroundView::update(double deltaTime)
{
	deltaTime_ = deltaTime;

	if (equipedItem_ && carryItem_)
	{
		equipedItem_->position    = mapWidget_.getAgentPosition();
		equipedItem_->position.x += mapWidget_.getTileScale().x * .5f * .5f;
		equipedItem_->position.y += mapWidget_.getTileScale().y;
		equipedItem_->position.z  = .4f;
		equipedItem_->updateTransform();
	}
}

void PlaygroundView::draw()
{
	mapWidget_.draw();
	inventoryWidget_.draw();
	if (equipedItem_)
	{
		spriteRenderer_.push(equipedItem_.value());
	}
	spriteRenderer_.draw();
}

Animation PlaygroundView::moveAgent(Pos2D oldPos, Pos2D newPos)
{
	glm::vec3 start  = mapWidget_.getTilePosition(oldPos);
	glm::vec3 target = mapWidget_.getTilePosition(newPos);

	return [this, start, target, progress = 0.0f]() mutable
	{
		const float duration = .5f / simulationSpeed_;

		progress += static_cast<float>(deltaTime_) / duration;

		float t = easeInOutCirc(progress);

		glm::vec3 position = glm::mix(start, target, t);
		position.z = 0.1f;
		mapWidget_.setAgentPosition(position);

		if (progress >= 1.0f)
		{
			target.z = .1f;
			mapWidget_.setAgentPosition(target);
			return Stop;
		}

		return Yield;
	};
}

Animation PlaygroundView::equipItem(Item item, ItemStash stash)
{
	return [
		this,
		item, stash,
		startPosition  = glm::vec3{.0f}, startScale  = glm::vec3{.0f},
		targetPosition = glm::vec3{.0f}, targetScale = glm::vec3{.0f},
		progress = 0.0f
	] () mutable
	{
		const float duration = 1.f / simulationSpeed_;

		if (progress == .0f)
		{
			startPosition = inventoryWidget_.getItemPosition(item);
			startScale    = inventoryWidget_.getItemScale();

			targetScale    = mapWidget_.getTileScale() * .5f;
			targetPosition = mapWidget_.getAgentPosition();
			targetPosition.x += mapWidget_.getTileScale().x * .5f * .5f;
			targetPosition.y += mapWidget_.getTileScale().y;

			inventoryWidget_.updateInventory(stash);

			equipedItem_ = inventoryWidget_.createItem(item);
		}

		progress += static_cast<float>(deltaTime_) / duration;

		float t = easeInOutCirc(progress);

		equipedItem_->position = glm::mix(startPosition, targetPosition, t);
		equipedItem_->scale    = glm::mix(startScale   , targetScale   , t);
		equipedItem_->position.z = 0.4f;
		equipedItem_->updateTransform();

		if (progress >= 1.0f)
		{	
			mapWidget_.setAgentCarry(true);
			carryItem_ = true;

			targetPosition.z = 0.4f;

			equipedItem_->position = targetPosition;
			equipedItem_->scale    = targetScale   ;
			equipedItem_->updateTransform();
			return Stop;
		}

		return Yield;
	};
}

Animation PlaygroundView::giveItem(Pos2D resident)
{
	return [
		this,
		resident,
		targetPosition = glm::vec3{0.}, targetScale = glm::vec3{0.},
		startPosition  = glm::vec3{0.}, startScale  = glm::vec3{0.},
		progress = 0.0f
	]() mutable
	{
		const float duration = .5f / simulationSpeed_;

		if (progress == 0.)
		{
			mapWidget_.setAgentCarry(false);
			carryItem_ = false;
			startPosition  = equipedItem_->position;
			startScale     = equipedItem_->scale;
			targetScale    = startScale * 0.25f;
			targetPosition = mapWidget_.getTilePosition(resident) + (mapWidget_.getTileScale() + targetScale) * .5f;
		}

		progress += static_cast<float>(deltaTime_) / duration;

		float t = easeInOutCirc(progress);

		equipedItem_->position = glm::mix(startPosition, targetPosition, t);
		equipedItem_->scale    = glm::mix(startScale   , targetScale   , t);
		equipedItem_->updateTransform();

		if (progress >= 1.0f)
		{
			equipedItem_ = std::nullopt;
			return Stop;
		}

		return Yield;
	};
}

Animation PlaygroundView::openChest()
{
	glm::vec3 startPosition  = inventoryWidget_.getClosePosition();
	glm::vec3 targetPosition = inventoryWidget_.getOpenPosition ();

	return [this, startPosition, targetPosition, progress=.0f]() mutable
	{
		const float duration = .75f / simulationSpeed_;

		if (progress == 0.f)
		{
			mapWidget_.openChest();
		}

		progress += static_cast<float>(deltaTime_) / duration;

		float t = easeInOutCirc(progress);

		glm::vec3 position = glm::mix(startPosition, targetPosition, t);
		position.z = 0.f;
		inventoryWidget_.setPosition(position);

		if (progress >= 1.f)
		{
			targetPosition.z = 0.f;
			inventoryWidget_.setPosition(targetPosition);
			return Stop;
		}
		
		return Yield;
	};
}

Animation PlaygroundView::closeChest()
{
	glm::vec3 startPosition  = inventoryWidget_.getOpenPosition ();
	glm::vec3 targetPosition = inventoryWidget_.getClosePosition();

	return [this, startPosition, targetPosition, progress=.0f]() mutable
	{
		const float duration = .75f / simulationSpeed_;

		progress += static_cast<float>(deltaTime_) / duration;

		float t = easeInOutCirc(progress);

		glm::vec3 position = glm::mix(startPosition, targetPosition, t);
		position.z = 0.f;
		inventoryWidget_.setPosition(position);

		if (progress >= 1.f)
		{
			mapWidget_.closeChest();
			targetPosition.z = 0.f;
			inventoryWidget_.setPosition(targetPosition);
			return Stop;
		}
		
		return Yield;
	};
}

Animation PlaygroundView::moveChestCursor(Pos2D old, Pos2D cursor)
{
	glm::vec3 start  = inventoryWidget_.getItemRelativePosition(old);
	glm::vec3 target = inventoryWidget_.getItemRelativePosition(cursor);

	return [this, start, target, progress=0.f]() mutable
	{
		const float duration = .5f / simulationSpeed_;

		if (progress == 0.f)
		{
			inventoryWidget_.showCursor();
		}

		progress += static_cast<float>(deltaTime_) / duration;

		float t = easeInOutCirc(progress);

		glm::vec3 position = glm::mix(start, target, t);
		position.z = .5f;
		inventoryWidget_.setCursorPosition(position);

		if (progress >= 1.f)
		{
			target.z = .5f;
			inventoryWidget_.setCursorPosition(target);
			return Stop;
		}

		return Yield;
	};
}