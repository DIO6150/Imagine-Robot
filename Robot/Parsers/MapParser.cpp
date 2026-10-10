

#include <Robot/Parsers/MapParser.hpp>
#include <Robot/Simulation/Map.hpp>

#include <Robot/Utils/JSONParser.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

MapParser::MapParser(Map & map, json const & object)
{
	JSONParser parser {
		Field::validate("format",
			[&map](json const & object) { return object.get<std::string>() == map.FORMAT_; }
		),
		Field::validate("version",
			[&map](json const & object) { return object.get<int>() == map.VERSION_; }
		),
		Field::fillNoError("nom",
			[&map](json const & object) { map.name_ = object.get<std::string>(); }
		),
		Field::branch("dimensions",
			{	// TODO: add type checking here
				Field::fillNoError("hauteur",
					[&map](json const & object) { map.height_ = static_cast<uint32_t>(object.get<int>()); }
				),
				Field::fillNoError("largeur",
					[&map](json const & object) { map.width_  = static_cast<uint32_t>(object.get<int>()); }
				)
			}
		),
		Field::fill("legende",
			[&map](json const & object)
			{
				for (auto const & nested : object.items())
				{
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
		Field::fill("grille",
			[&map](json const & object)
			{
				for (int32_t y = 0; y < map.height_; ++y)
				{
					std::string const & row = object.at(y).get_ref<std::string const &>();

					for (int32_t x = 0; x < map.width_; ++x)
					{
						auto tileID = row.at(x);

						if (!map.charProperties_.contains(tileID)) return JSONParserStatus::UnknownProperty;

						map.tiles_.emplace_back(Tile { map.charProperties_.at(tileID), Pos2D {x, y} });
					}
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("depart_robot",
			[&map](json const & object)
			{
				Pos2D robotStart; // obviously should check if its a list and if it has size == 2
				robotStart.x = object.at(0).get<int>();
				robotStart.y = object.at(1).get<int>();

				auto tile  = map.getTile(robotStart);

				if (!tile.isAgentStart())
					return JSONParserStatus::IncoherentData;

				map.robotStart_ = robotStart;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("armoire",
			[&map](json const & object)
			{
				Pos2D stash; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				stash.x = object.at("position").at(0).get<int>();
				stash.y = object.at("position").at(1).get<int>();

				auto tile = map.getTile(stash);

				if (!tile.isItemPickup())
					return JSONParserStatus::IncoherentData; // obv not everyhing is checkable at validator layer so eeeh

				map.stash_ = stash;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("dictionnaire",
			[&map](json const & object)
			{
				Pos2D dict; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				dict.x = object.at("position").at(0).get<int>();
				dict.y = object.at("position").at(1).get<int>();

				auto tile = map.getTile(dict);

				if (!tile.isRecord())
					return JSONParserStatus::IncoherentData; // obv not everyhing is checkable at validator layer so eeeh

				map.dictionary_ = dict;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("residents",
			[&map](json const & object)
			{
				for (auto const & nested : object)
				{
					Resident resident;

					resident.id_   = nested.at("id") .get<std::string>();
					resident.name_ = nested.at("nom").get<std::string>();


					resident.pos_.x = nested.at("position").at(0).get<int>();
					resident.pos_.y = nested.at("position").at(1).get<int>();

					auto tile = map.getTile(resident.pos_);

					if (!tile.isPerson())
						return JSONParserStatus::IncoherentData;

					map.residentList_.push_back(resident);
				}

				return JSONParserStatus::Ok;
			}
		)
	};

	result = parser.fillFields(object);
}