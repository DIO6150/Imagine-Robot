#pragma once

#include <stdint.h>

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include <Robot/Common/Emotion.hpp>

#include <Robot/Errors/Error.hpp>

#include <Robot/Utils/JSONParser.hpp>
#include <Robot/Utils/Pos.hpp>

#include <vendor/nlohmann/json.hpp>

struct Drawer
{
	Pos2D location_;
	Emotion emotion_;
	Intensity intensity_;
	std::string item_;
};

class ItemStash
{
public:
	JSONParserStatus parseJSON(json const & object);

	std::string getItem(Pos2D pos);
	Pos2D getCasierDepart();
	void closetInfos();
	void removeItem(Pos2D posDrawer, std::string itemName);

private:
	std::string name_;
	std::vector<Drawer> drawers_ = std::vector<Drawer>(24);
	
	std::string const FORMAT_  = "robot-reconfort/armoire";
	int         const VERSION_ = 1;
	Pos2D casierDepart_;
	std::map<std::string, Emotion> stringToEmotions_;
	std::map<std::string, Intensity> stringToIntensities_;

	friend class ClosetParser;
};

