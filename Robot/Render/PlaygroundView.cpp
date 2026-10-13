
#include <Robot/Render/PlaygroundView.hpp>

#include <glad/glad.h>



PlaygroundView::PlaygroundView(PlaygroundViewListener * listener)
	: listener_ {listener}
{
	
}

void PlaygroundView::init()
{

}

void PlaygroundView::update()
{

}

void PlaygroundView::draw()
{
	/*
		Draw Map here
	*/
	for (auto const & tile : map_ )
	{
		tile.draw();
	}

	/*
		Draw Agent here
	*/
	agent_.draw();

	/*
		Draw Item Stash here
	*/
}
