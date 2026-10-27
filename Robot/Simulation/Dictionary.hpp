#pragma once

#include <stdint.h>
#include <iostream>

#include <string>
#include <map>
#include <tuple>
#include <vector>

#include <Robot/Common/Emotion.hpp>

#include <Robot/Errors/Error.hpp>

#include <Robot/Utils/JSONParser.hpp>

#include <vendor/nlohmann/json.hpp>

struct Entree
{
	Emotion emotion_;
    Intensity intensity_;
};

class Dictionary
{
public:
	JSONParserStatus parseJSON(json const & object);
	std::map<std::string, Entree> getEntrees();
	static std::tuple<Emotion, Intensity> identifEmoIntens(std::string message, std::map<std::string, Entree> givenRecord);
	void dictionaryInfos();

private:
	std::string name_;
	std::map<std::string, Entree> entrees_;
	
	std::string const FORMAT_  = "robot-reconfort/dictionnaire";
	int         const VERSION_ = 1;
    
	std::map<std::string, Emotion> stringToEmotions_;
	std::map<std::string, Intensity> stringToIntensities_;

	friend class DictionaryParser;
};

