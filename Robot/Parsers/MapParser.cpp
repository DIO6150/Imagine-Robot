

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
				uint32_t nElementsInLine = (object.at(0).get<std::string>()).size();
				uint32_t nLines = object.size();

				if(nLines != map.height_ || nElementsInLine != map.width_)
					return JSONParserStatus::InacurrateDimensions;
				
				for (int32_t l = 0; l < map.height_; ++l)
				{
					std::string const & row = object.at(l).get<std::string>();
					if (row.size() != nElementsInLine)
						return JSONParserStatus::GridNotRectangular;

					for (int32_t c = 0; c < map.width_; ++c)
					{
						auto tileID = row.at(c);

						if (!map.charProperties_.contains(tileID)) return JSONParserStatus::UnknownProperty;

						map.tiles_.emplace_back(Tile { map.charProperties_.at(tileID), Pos2D {l, c} });
					}
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("depart_robot",
			[&map](json const & object)
			{
				Pos2D robotStart; // obviously should check if its a list and if it has size == 2
				robotStart.l = object.at(0).get<int>();
				robotStart.c = object.at(1).get<int>();

				auto tile  = map.getTile(robotStart);

				if (!tile.isAgentStart())
					return JSONParserStatus::InacurratePosition;

				if (!map.isInside(robotStart))
					return JSONParserStatus::PositionOutOfGrid;

				map.robotStartPos_ = robotStart;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("armoire",
			[&map](json const & object)
			{
				Pos2D stash; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				stash.l = object.at("position").at(0).get<int>();
				stash.c = object.at("position").at(1).get<int>();

				auto tile = map.getTile(stash);

				if (!tile.isItemPickup())
					return JSONParserStatus::InacurratePosition; // obv not everyhing is checkable at validator layer so eeeh

				if (!map.isInside(stash))
					return JSONParserStatus::PositionOutOfGrid;

				map.stashPos_ = stash;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("dictionnaire",
			[&map](json const & object)
			{
				Pos2D dict; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				dict.l = object.at("position").at(0).get<int>();
				dict.c = object.at("position").at(1).get<int>();

				auto tile = map.getTile(dict);

				if (!tile.isRecord())
					return JSONParserStatus::InacurratePosition; // obv not everyhing is checkable at validator layer so eeeh

				if (!map.isInside(dict))
					return JSONParserStatus::PositionOutOfGrid;

				map.dictionaryPos_ = dict;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("residents",
			[&map](json const & object)
			{
				for (auto const & nested : object)
				{
					Resident resident;

					resident.name_ = nested.at("nom").get<std::string>();
					resident.pos_.l = nested.at("position").at(0).get<int>();
					resident.pos_.c = nested.at("position").at(1).get<int>();

					auto tile = map.getTile(resident.pos_);

					if (!tile.isPerson())
						return JSONParserStatus::InacurratePosition;

					if (!map.isInside(resident.pos_))
						return JSONParserStatus::PositionOutOfGrid;

					if(map.residents_.contains(nested.at("id").get<std::string>()))
						return JSONParserStatus::ResidentSharedId;
					
					map.residents_.emplace(nested.at("id").get<std::string>(), resident);
				}

				return JSONParserStatus::Ok;
			}
		)
	};

	result = parser.fillFields(object);
}
