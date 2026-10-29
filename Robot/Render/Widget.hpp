#pragma once

#include <Robot/Render/SpriteRenderer.hpp>

struct NeededResources
{
	std::vector<std::string> meshes;
	std::vector<std::string> textures;
	std::vector<std::string> shaders;
};

class AbstractWidget
{
public:
	AbstractWidget (SpriteRenderer & renderer, SharedResourceArray & resourceArray)
		: renderer_      {renderer}
		, resourceArray_ {resourceArray}
	{

	}

	bool canInitialize()
	{
		auto needed = onResourcePreLoad();

		for (auto const & key : needed.meshes)
		{
			if (!resourceArray_.getMesh(key)) return false;
		}

		for (auto const & key : needed.textures)
		{
			if (!resourceArray_.getTexture(key)) return false;
		}

		for (auto const & key : needed.shaders)
		{
			if (!resourceArray_.getProgram(key)) return false;
		}

		return true;
	}

	void draw()
	{
		onRender();
		
		//renderer_.draw();
	}

protected:
	virtual void onUpdate() {}
	virtual void onRender() {}
	virtual NeededResources onResourcePreLoad() = 0;

protected:
	Mesh getMesh(std::string key)
	{
		return resourceArray_.getMesh(key).value();
	}

	GLuint getTexture(std::string key)
	{
		return resourceArray_.getTexture(key).value();
	}

	ShaderProgram getShader(std::string key)
	{
		return resourceArray_.getProgram(key).value();
	}


protected:
	SpriteRenderer & renderer_;
	SharedResourceArray & resourceArray_;
};