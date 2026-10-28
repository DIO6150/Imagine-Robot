
#include <Robot/Render/ItemRenderer.hpp>

#include <Robot/Render/Image.hpp>

void ItemRenderer::init(SharedResources * resources)
{
	resources_ = resources;

	Image joy, confidence, fear, surprised, sadness, disgust, anger, anticipation;

	joy         .load("data/assets/textures/emotion_joy.png");
	confidence  .load("data/assets/textures/emotion_confidence.png");
	fear        .load("data/assets/textures/emotion_fear.png");
	surprised   .load("data/assets/textures/emotion_surpised.png");
	sadness     .load("data/assets/textures/emotion_sadness.png");
	disgust     .load("data/assets/textures/emotion_disgust.png");
	anger       .load("data/assets/textures/emotion_anger.png");
	anticipation.load("data/assets/textures/emotion_anticipation.png");

	textures_[Emotion::Joy]          = joy         .createTexture();
	textures_[Emotion::Confidence]   = confidence  .createTexture();
	textures_[Emotion::Fear]         = fear        .createTexture();
	textures_[Emotion::Surprised]    = surprised   .createTexture();
	textures_[Emotion::Sadness]      = sadness     .createTexture();
	textures_[Emotion::Disgust]      = disgust     .createTexture();
	textures_[Emotion::Anger]        = anger       .createTexture();
	textures_[Emotion::Anticipation] = anticipation.createTexture();

	colors_[Intensity::Small]  = { 1.f, .7f, .4f };
	colors_[Intensity::Mid]    = { 1.f, .6f, .3f };
	colors_[Intensity::Strong] = { .6f, 3.f, .0f };
}

ItemInstance ItemRenderer::createItem(Emotion emotion, Intensity intensity)
{
	ItemInstance instance;

	instance.emotionTexture = textures_.at(emotion);
	instance.intensityColor = colors_  .at(intensity);

	return instance;
}

void ItemRenderer::render(ItemInstance item)
{
	renderList_.emplace_back(item);
}

void ItemRenderer::draw()
{
	glBindVertexArray(resources_->rectVao);
	resources_->rectProgram.use();
	for (auto const & item : renderList_)
	{
		resources_->rectProgram.uploadMatrix4("u_transform", item.transform);

		resources_->rectProgram.upload3fv("u_color", item.intensityColor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, item.emotionTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	resources_->rectProgram.cancel();
	glBindVertexArray(0);

	renderList_.clear();
}
