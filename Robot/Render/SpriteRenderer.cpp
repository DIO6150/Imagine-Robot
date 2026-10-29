
#include <Robot/Render/SpriteRenderer.hpp>

#include <Robot/Render/Image.hpp>

#include <glfw/glfw3.h>

#include <algorithm>

void Sprite::updateTransform()
{
	transform = glm::translate(glm::mat4 {1.}, position);
	transform = glm::scale(transform, scale);
}

SpriteRenderer::SpriteRenderer(SharedResourceArray & resources)
	: resourceArray_ {resources}
{

	//Image joy, confidence, fear, surprised, sadness, disgust, anger, anticipation;
//
	//joy         .load("data/assets/textures/emotion_joy.png");
	//confidence  .load("data/assets/textures/emotion_confidence.png");
	//fear        .load("data/assets/textures/emotion_fear.png");
	//surprised   .load("data/assets/textures/emotion_surpised.png");
	//sadness     .load("data/assets/textures/emotion_sadness.png");
	//disgust     .load("data/assets/textures/emotion_disgust.png");
	//anger       .load("data/assets/textures/emotion_anger.png");
	//anticipation.load("data/assets/textures/emotion_anticipation.png");
//
	//textures_[Emotion::Joy]          = joy         .createTexture();
	//textures_[Emotion::Confidence]   = confidence  .createTexture();
	//textures_[Emotion::Fear]         = fear        .createTexture();
	//textures_[Emotion::Surprised]    = surprised   .createTexture();
	//textures_[Emotion::Sadness]      = sadness     .createTexture();
	//textures_[Emotion::Disgust]      = disgust     .createTexture();
	//textures_[Emotion::Anger]        = anger       .createTexture();
	//textures_[Emotion::Anticipation] = anticipation.createTexture();
//
	//colors_[Intensity::Small]  = { 1.f, .7f, .4f };
	//colors_[Intensity::Mid]    = { 1.f, .6f, .3f };
	//colors_[Intensity::Strong] = { .6f, 3.f, .0f };
}

void SpriteRenderer::initializeDefaults()
{
	std::vector<float> vertices = {
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f
	};

	resourceArray_.createMesh("default", vertices, SharedResourceArray::Vertex | SharedResourceArray::UV);

	Image image;
	image.loadSinglePixelTexture({1., 1., 1.});

	resourceArray_.createTexture("default", image);

	resourceArray_.createProgram("default", TILE_VERTEX_SHADER, TILE_FRAGMENT_SHADER);
}

void SpriteRenderer::setProjectionMatrix(glm::mat4 projection)
{
	projection_ = projection;
}

std::optional<Sprite> SpriteRenderer::createSprite(std::string meshKey, std::string shaderKey, std::string textureKey)
{
	auto mesh = resourceArray_.getMesh(meshKey);
	if (!mesh) return std::nullopt;

	auto texture = resourceArray_.getTexture(textureKey);
	if (!texture) return std::nullopt;

	auto shader = resourceArray_.getProgram(shaderKey);
	if (!shader) return std::nullopt;

	Sprite instance;
	instance.vao         = mesh->vao;
	instance.vertexCount = mesh->vertexCount;
	instance.shader   = shaderKey;
	instance.texture  = texture.value();
	instance.color    = {1.0, 1.0, 1.0};
	instance.position = { .0,  .0,  .0};
	instance.scale    = {1.0, 1.0, 1.0};
	instance.updateTransform();

	return instance;
}

std::optional<Sprite> SpriteRenderer::createSprite(std::string meshKey, std::string shaderKey)
{
	return createSprite(meshKey, shaderKey, "default");
}

void SpriteRenderer::push(Sprite instance)
{
	renderList_.emplace_back(instance);
}

void SpriteRenderer::draw()
{
	auto graph = sortRenderList();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	for (auto vaoIndex = 0; vaoIndex < graph.vaos.size(); ++vaoIndex)
	{
		auto & vao = graph.vaos.at(vaoIndex);
		auto & programList = graph.programs.at(vaoIndex);
		auto & instanceListList = graph.instances.at(vaoIndex);

		glBindVertexArray(vao);

		for (auto programIndex = 0; programIndex < programList.size(); ++programIndex)
		{
			auto & program = programList.at(programIndex);
			auto & instanceList = instanceListList.at(programIndex);

			program.use();

			for (auto instanceIndex = 0; instanceIndex < instanceList.size(); ++instanceIndex)
			{
				auto & instance = instanceList.at(instanceIndex);

				program.uploadMatrix4("u_projection", projection_);
				program.uploadMatrix4("u_transform" , instance.transform);
				program.upload3fv    ("u_color"     , instance.color);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture  (GL_TEXTURE_2D, instance.texture);

				glDrawArrays(GL_TRIANGLES, 0, instance.vertexCount);
			}

			program.cancel();
		}

		glBindVertexArray(0);
	}

	renderList_.clear();
}

SpriteRenderer::Graph SpriteRenderer::sortRenderList()
{
    Graph result;

    // Utilisation d'une map pour regrouper les sprites : VAO -> (ShaderKey -> Liste de Sprites)
    std::map<GLuint, std::map<std::string, std::vector<Sprite>>> groupedSprites;

	// Supposons que vous ayez accès à votre matrice View-Projection dans le renderer

	std::stable_sort(renderList_.begin(), renderList_.end(), [this](const Sprite& a, const Sprite& b) {
		// Calcul de la position dans l'espace de projection (Clip Space)
		glm::vec4 posA = projection_ * a.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 posB = projection_ * b.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		// Comparaison de la coordonnée Z normalisée (profondeur)
		return posA.z > posB.z; 
	});

    for (const auto& sprite : renderList_)
    {
        groupedSprites[sprite.vao][sprite.shader].push_back(sprite);
    }

    // Remplissage de la structure Graph
    for (const auto& [vao, shaderMap] : groupedSprites)
    {
        result.vaos.push_back(vao);

        std::vector<ShaderProgram> programList;
        std::vector<std::vector<Sprite>> instanceListList;

        for (const auto& [shaderKey, spriteList] : shaderMap)
        {
            auto shader = resourceArray_.getProgram(shaderKey);
            if (shader)
            {
                // Récupération de l'objet ShaderProgram (fonctionne que getProgram retourne un pointeur ou un std::optional)
                programList.push_back(*shader);
                instanceListList.push_back(spriteList);
            }
        }

        result.programs.push_back(programList);
        result.instances.push_back(instanceListList);
    }

    return result;
}