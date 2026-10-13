#pragma once

#include <Robot/Render/Meshes/Rect.hpp>
#include <Robot/Utils/Color.hpp>

#include <Robot/Common/Tile.hpp>

#include <Robot/Render/Shaders/BasicVertexShader.hpp>
#include <Robot/Render/Shaders/BasicFragmentShader.hpp>

struct TileMesh : Rect
{
	Color backgroundColor;
	Color outlineColor;
	bool  outline = false;

	static TileMesh generate(Tile tile)
	{
		TileMesh result;
		result.initBuffers();
		result.setShader(BASIC_VERTEX_SHADER, BASIC_VERTEX_SHADER); // urgghhhh

		if (tile.isSolid())
			result.backgroundColor = { 0.f, 0.f, 1.f };
		else
			result.backgroundColor = { .0f, .0f, .0f };

		return result;
	}
};