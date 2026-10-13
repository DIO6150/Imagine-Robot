#pragma once

#include <iostream>
#include <stdint.h>

#include <string>
#include <map>
#include <vector>

#include <Robot/Common/Request.hpp>
#include <Robot/Errors/Error.hpp>

#include <Robot/Utils/JSONParser.hpp>

#include <vendor/nlohmann/json.hpp>

class Script
{
public:
	JSONParserStatus parseJSON(json const & object);
	void scriptInfos();
    std::vector<Request> getRequests();
	std::string getMapName();
	std::string getClosetName();

private:
	std::string name_;
	std::vector<Request> requests_;
	
	std::string const FORMAT_  = "robot-reconfort/scenario";
	int         const VERSION_ = 1;
	
	std::string mapName_;
	std::string closetName_;

	friend class ScriptParser;
};

