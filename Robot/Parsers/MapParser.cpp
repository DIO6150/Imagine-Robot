

#include <Robot/Parsers/MapParser.hpp>

using json = nlohmann::json;

MapParser::MapParser(Map & map, json const & object)
{
	JSONParser parser {
		{"format", "version", "nom", "dimensions", "legende", "grille",
			"depart_robot", "armoire", "dictionnaire", "residents"},
	{
		Field::validate("format",
			[&map](json const & object) 
			{ 
				if (!object.is_string())
					return JSONParserStatus::WrongType;
				if (object.get<std::string>() != map.FORMAT_)
					return JSONParserStatus::WrongFormat;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("version",
			[&map](json const & object) 
			{ 
				if (!object.is_number_integer())
					return JSONParserStatus::WrongType;
				if (object.get<int>() != map.VERSION_)
					return JSONParserStatus::WrongVersion;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("nom", JSONParser::verifySimpleString),
		Field::fillNoError("nom",
			[&map](json const & object) { map.name_ = object.get<std::string>(); }
		),
		Field::branch("dimensions",
			{
				Field::validate("hauteur",
					[](json const & object) 
					{ 
						if (!object.is_number_integer())
							return JSONParserStatus::WrongType;
						if (object.get<int>() < 1)
							return JSONParserStatus::GridNotRectangular;
						return JSONParserStatus::Ok;
					}
				),
				Field::validate("largeur",
					[](json const & object) 
					{ 
						if (!object.is_number_integer())
							return JSONParserStatus::WrongType;
						if (object.get<int>() < 1)
							return JSONParserStatus::GridNotRectangular;
						return JSONParserStatus::Ok;
					}
				)
			}
		),
		Field::validate("dimensions",
			[](json const & object) 
			{ 
				if (!object.is_object())
					return JSONParserStatus::WrongType;
				
				if(!object.contains("hauteur") || !object.contains("largeur"))
					return JSONParserStatus::MissingField;
				
				return JSONParserStatus::Ok;
			}
		),
		Field::fillNoError("dimensions",
			[&map](json const & object)
			{
				map.height_ = static_cast<uint32_t>(object["hauteur"].get<int>());
				map.width_  = static_cast<uint32_t>(object["largeur"].get<int>());
			}
		),
		Field::fill("legende",
			[&map](json const & object)
			{
				map.charProperties_.clear();
				if(!object.is_object())
					return JSONParserStatus::WrongType;
				
				for (auto const & nested : object.items())
				{
					if (!nested.value().is_string())
						return JSONParserStatus::WrongType;

					auto value = nested.value().get<std::string>();
					TileProperty property;

					if (value == "libre") {} // do nothing
					else if (value == "depart du robot") property.setAgentStart(true);
					else if (value == "mur")             property.setSolid(true);
					else if (value == "armoire")         property.setSolid(true).setItemPickup(true);
					else if (value == "dictionnaire")    property.setSolid(true).setRecord    (true);
					else if (value == "resident")        property.setSolid(true).setPerson    (true);
					else return JSONParserStatus::UnknownProperty;

					map.charProperties_.emplace(nested.key().at(0), property);
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("grille",
			[](json const & object)
			{
				if (!object.is_array() || object.empty())
					return JSONParserStatus::WrongType;

				int hauteurGrille = object.size();

				if (object.at(0).empty())
					return JSONParserStatus::GridNotRectangular;

				int largeurGrille = object.at(0).get<std::string>().size();

				for (int i = 0; i < hauteurGrille; i++)
				{
					if (!object.at(i).is_string())
						return JSONParserStatus::WrongType;
					if (object.at(i).get<std::string>().size() != largeurGrille)
						return JSONParserStatus::GridNotRectangular;
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::fill("grille",
			[&map](json const & object)
			{
				map.tiles_.clear();
				
				uint32_t nElementsInLine = (object.at(0).get<std::string>()).size();
				uint32_t nLines = object.size();

				if(nLines != map.height_ || nElementsInLine != map.width_)
					return JSONParserStatus::InacurrateDimensions;
				
				for (int32_t l = 0; l < map.height_; ++l)
				{
					std::string const & row = object.at(l).get<std::string>();

					for (int32_t c = 0; c < map.width_; ++c)
					{
						auto tileID = row.at(c);

						if (!map.charProperties_.contains(tileID)) return JSONParserStatus::UnknownProperty;

						map.tiles_.push_back(Tile { map.charProperties_.at(tileID), Pos2D {l, c} });
					}
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::validate("depart_robot", JSONParser::verifySimplePos),
		Field::fill("depart_robot",
			[&map](json const & object)
			{
				Pos2D robotStart; // obviously should check if its a list and if it has size == 2
				robotStart.l = object.at(0).get<int>();
				robotStart.c = object.at(1).get<int>();

				if (!map.isInside(robotStart))
					return JSONParserStatus::PositionOutOfGrid;
				
				auto tile  = map.getTile(robotStart);

				if (!tile.isAgentStart())
					return JSONParserStatus::InacurratePosition;

				map.robotStartPos_ = robotStart;

				return JSONParserStatus::Ok;
			}
		),
		Field::validate("armoire", JSONParser::verifySimplePos),
		Field::fill("armoire",
			[&map](json const & object)
			{
				Pos2D stash; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				stash.l = object.at("position").at(0).get<int>();
				stash.c = object.at("position").at(1).get<int>();

				if (!map.isInside(stash))
					return JSONParserStatus::PositionOutOfGrid;

				auto tile = map.getTile(stash);

				if (!tile.isItemPickup())
					return JSONParserStatus::InacurratePosition; // obv not everyhing is checkable at validator layer so eeeh

				map.stashPos_ = stash;

				return JSONParserStatus::Ok;
			}
		),
		Field::validate("dictionnaire", JSONParser::verifySimplePos),
		Field::fill("dictionnaire",
			[&map](json const & object)
			{
				Pos2D dict; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				dict.l = object.at("position").at(0).get<int>();
				dict.c = object.at("position").at(1).get<int>();

				if (!map.isInside(dict))
					return JSONParserStatus::PositionOutOfGrid;

				auto tile = map.getTile(dict);

				if (!tile.isRecord())
					return JSONParserStatus::InacurratePosition; // obv not everyhing is checkable at validator layer so eeeh

				map.dictionaryPos_ = dict;

				return JSONParserStatus::Ok;
			}
		),
		Field::branch("residents",
			{
				Field::validate("id", JSONParser::verifySimpleString),
				Field::validate("nom", JSONParser::verifySimpleString),
				Field::validate("position", JSONParser::verifySimplePos),
			}
		),
		Field::validate("residents",
			[](json const & object)
			{
				if(!object.is_array() || object.empty())
					return JSONParserStatus::WrongType;

				for (auto const & nested : object)
				{
					if(!nested.is_object())
						return JSONParserStatus::WrongType;

					if(!nested.contains("id") || !nested.contains("nom") || !nested.contains("position"))
						return JSONParserStatus::MissingField;
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::fill("residents",
			[&map](json const & object)
			{
				map.residents_.clear();
				for (auto const & nested : object)
				{
					Resident resident;

					resident.name_ = nested.at("nom").get<std::string>();
					resident.pos_.l = nested.at("position").at(0).get<int>();
					resident.pos_.c = nested.at("position").at(1).get<int>();

					if (!map.isInside(resident.pos_))
						return JSONParserStatus::PositionOutOfGrid;

					auto tile = map.getTile(resident.pos_);

					if (!tile.isPerson())
						return JSONParserStatus::InacurratePosition;

					if(map.residents_.contains(nested.at("id").get<std::string>()))
						return JSONParserStatus::ResidentSharedId;
					
					map.residents_.emplace(nested.at("id").get<std::string>(), resident);
				}

				return JSONParserStatus::Ok;
			}
		)
	}
	};

	result = parser.fillFields(object);
}
