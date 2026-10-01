
#include <Robot/Simulation/Map.hpp>

using json = nlohmann::json;

Error Map::parseJson(json object)
{
	// TODO add more descriptive errors

	if (!object.contains("format" )) return Error::JSONSyntaxError;
	if (!object.contains("version")) return Error::JSONSyntaxError;
	if (!object.contains("nom"    )) return Error::JSONSyntaxError;
	if (object.contains("dimensions"))
	{
		if (!object.at("dimensions").contains("hauteur"))
			return Error::JSONSyntaxError;

		if (!object.at("dimensions").contains("largeur"))
			return Error::JSONSyntaxError;
	}
	else return Error::JSONSyntaxError;

	auto format  = object.at("format") .get<std::string>();
	auto version = object.at("version").get<int>();

	if (version != 1)
		return Error::JSONSyntaxError;

	if (format != "robot-reconfort/carte")
		return Error::JSONSyntaxError;

	name_  = object.at("nom").get<std::string>();

	auto size = object.at("dimensions");
	width_  = size.at("largeur");
	height_ = size.at("hauteur");


}