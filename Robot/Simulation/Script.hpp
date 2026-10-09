#pragma once

#include <iostream>
#include <stdint.h>

#include <string>
#include <map>
#include <vector>

#include <Robot/Common/Request.hpp>
#include <Robot/Errors/Error.hpp>

#include <Robot/Utils/JSONParser.hpp>
#include <Robot/Utils/Pos.hpp>

#include <vendor/nlohmann/json.hpp>

class Script
{
public:
	JSONParserStatus parseJSON(json const & object);
    std::vector<Request> getRequests();

private:
	std::string name_;
	std::vector<Request> requests_;
	
	std::string const FORMAT_  = "robot-reconfort/scenario";
	int         const VERSION_ = 1;
	
	std::string mapName_;
	std::string closetName_;

	JSONParser parser_ {
		Field::validate("format",
			[this](json const & object) { return object.get<std::string>() == FORMAT_; }
		),
		Field::validate("version",
			[this](json const & object) { return object.get<int>() == VERSION_; }
		),
		Field::fillNoError("nom",
			[this](json const & object) { name_ = object.get<std::string>(); }
		),
		Field::fillNoError("carte",
			[this](json const & object) { mapName_ = object.get<std::string>(); }
		),
		Field::fillNoError("armoire",
			[this](json const & object) { closetName_ = object.get<std::string>(); }
		),
		Field::fill("demandes",
			[this](json const & object)
			{
				for (auto const & nested : object)
				{
					Request request = Request(nested.at("numero").get<int>(), nested.at("resident").get<std::string>(), nested.at("message").get<std::string>());
					requests_.push_back(request);
				}
				
				return JSONParserStatus::Ok;
			}
		)
	};
};
