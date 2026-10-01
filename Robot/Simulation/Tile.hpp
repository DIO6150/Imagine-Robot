#pragma once

#include <stdint.h>

#include <string>

class TileProperty
{
public:
	void solid();
	void agentStart();
	void person();
	void item();

	bool isSolid();
	bool isAgentStart();
	bool isPerson();
	bool isItem();

private:
	uint64_t flags_;
};

struct Tile
{
	TileProperty property;
	std::string alias;
	char icon;

	uint32_t posX_;
	uint32_t posY_;
};
