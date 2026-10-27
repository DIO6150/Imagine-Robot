
#include <Robot/Render/PlaygroundView.hpp>

#include <glad/glad.h>



PlaygroundView::PlaygroundView(PlaygroundViewListener * listener)
	: listener_ {listener}
{
	
}

void PlaygroundView::init(Map const & map)
{
	physicalMap_.init();
	physicalMap_.setMap(map.rawTileMap(), map.getWidth(), map.getHeight());
}

void PlaygroundView::update()
{

}

void PlaygroundView::draw()
{
	/*
		Draw Map here
	*/
	physicalMap_.draw();

	/*
		Draw Agent here
	*/

	/*
		Draw Item Stash here
	*/
}
