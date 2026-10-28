

#include <Robot/Parsers/ScriptParser.hpp>

using json = nlohmann::json;

ScriptParser::ScriptParser(Script & script, json const & object)
{
	JSONParser parser {
		{"format", "version", "nom", "carte", "armoire", "demandes"},
		{
		Field::validate("format",
			[&script](json const & object) 
			{ 
				if (!object.is_string())
					return JSONParserStatus::WrongType;
				if (object.get<std::string>() != script.FORMAT_)
					return JSONParserStatus::WrongFormat;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("version",
			[&script](json const & object) 
			{ 
				if (!object.is_number_integer())
					return JSONParserStatus::WrongType;
				if (object.get<int>() != script.VERSION_)
					return JSONParserStatus::WrongVersion;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("nom", JSONParser::verifySimpleString),
		Field::fillNoError("nom",
			[&script](json const & object) { script.name_ = object.get<std::string>(); }
		),
		Field::validate("carte", JSONParser::verifySimpleString),
		Field::fillNoError("carte",
			[&script](json const & object) { script.mapName_ = object.get<std::string>(); }
		),
		Field::validate("armoire", JSONParser::verifySimpleString),
		Field::fillNoError("armoire",
			[&script](json const & object) { script.closetName_ = object.get<std::string>(); }
		),
		Field::branch("demandes",
			{
				Field::validate("numero",
					[&script](json const & object) 
					{ 
						if (!object.is_number_integer())
							return JSONParserStatus::WrongType;
						return JSONParserStatus::Ok;
					}
				),
				Field::validate("resident", JSONParser::verifySimpleString),
				Field::validate("message",JSONParser::verifySimpleString)
			}
		),
		Field::validate("demandes",
			[](json const & object)
			{
				if(!object.is_array() || object.empty())
					return JSONParserStatus::WrongType;

				for (auto const & nested : object)
				{
					if(!nested.is_object())
						return JSONParserStatus::WrongType;

					if(!nested.contains("numero") || !nested.contains("resident") || !nested.contains("message"))
						return JSONParserStatus::MissingField;
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::fillNoError("demandes",
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
	}
	};

	result = parser.fillFields(object);
}
