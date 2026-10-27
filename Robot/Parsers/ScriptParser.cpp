

#include <Robot/Parsers/ScriptParser.hpp>

using json = nlohmann::json;

ScriptParser::ScriptParser(Script & script, json const & object)
{
	JSONParser parser {
		Field::validate("format",
			[&script](json const & object) { return object.get<std::string>() == script.FORMAT_; }
		),
		Field::validate("version",
			[&script](json const & object) { return object.get<int>() == script.VERSION_; }
		),
		Field::fillNoError("nom",
			[&script](json const & object) { script.name_ = object.get<std::string>(); }
		),
		Field::fillNoError("carte",
			[&script](json const & object) { script.mapName_ = object.get<std::string>(); }
		),
		Field::fillNoError("armoire",
			[&script](json const & object) { script.closetName_ = object.get<std::string>(); }
		),
		Field::fill("demandes",
			[&script](json const & object)
			{
				for (auto const & nested : object)
				{
					Request request = Request(nested.at("numero").get<int>(), nested.at("resident").get<std::string>(), nested.at("message").get<std::string>());
					script.requests_.push_back(request);
				}
				
				return JSONParserStatus::Ok;
			}
		)
	};

	result = parser.fillFields(object);
}
