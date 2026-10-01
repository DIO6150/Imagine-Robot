#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include <Robot/Errors/Error.hpp>

#include <Robot/Simulation/Tile.hpp>
#include <Robot/Simulation/Resident.hpp>

#include <Robot/Utils/JSONParser.hpp>
#include <Robot/Utils/Pos.hpp>

#include <vendor/nlohmann/json.hpp>

class Map
{
public:
	JSONParserStatus parseJson(json const & object);

	Tile getTile(int32_t x, int32_t y) const;
	Tile getTile(Pos2D pos) const;

private:
	std::string const FORMAT_  = "robot-reconfort/carte";
	int         const VERSION_ = 1;

	std::string name_;
	uint32_t width_;
	uint32_t height_;
	std::map<char, TileProperty> charProperties_;
	std::vector<Tile> tiles_;
	Pos2D robotStart_;
	Pos2D stash_;
	Pos2D dictionary_;
	std::vector<Resident> residentList_;

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
		Field::branch("dimensions",
			{	// TODO: add type checking here
				Field::fillNoError("hauteur",
					[this](json const & object) { height_ = static_cast<uint32_t>(object.get<int>()); }
				),
				Field::fillNoError("largeur",
					[this](json const & object) { width_  = static_cast<uint32_t>(object.get<int>()); }
				)
			}
		),
		Field::fill("legende",
			[this](json const & object)
			{
				for (auto const & nested : object.items())
				{
					auto value = nested.value().get<std::string>();
					TileProperty property;

					if (value == "libre") {} // do nothing
					else if (value == "depart du robot") property.setAgentStart(true);
					else if (value == "mur")             property.setSolid(true);
					else if (value == "armoire")         property.setSolid(true).setItemPickup  (true);
					else if (value == "dictionnaire")    property.setSolid(true).setRecord(true);
					else if (value == "resident")        property.setSolid(true).setPerson      (true);
					else return JSONParserStatus::UnknownProperty;
					// it will however break all other filler functions

					charProperties_.emplace(nested.key().at(0), property);

				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("grille",
			[this](json const & object)
			{
				for (int32_t y = 0; y < height_; ++y)
				{
					std::string const & row = object.at(y).get_ref<std::string const &>();

					for (int32_t x = 0; x < width_; ++x)
					{
						auto tileID = row.at(x);

						if (!charProperties_.contains(tileID)) return JSONParserStatus::UnknownProperty;

						tiles_.emplace_back(Tile { charProperties_.at(tileID), Pos2D {x, y} });
					}
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("depart_robot",
			[this](json const & object)
			{
				Pos2D robotStart; // obviously should check if its a list and if it has size == 2
				robotStart.x = object.at(0).get<int>();
				robotStart.y = object.at(1).get<int>();

				auto tile  = getTile(robotStart);

				if (!tile.isAgentStart())
					return JSONParserStatus::IncoherentData;

				robotStart_ = robotStart;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("armoire",
			[this](json const & object)
			{
				Pos2D stash; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				stash.x = object.at("position").at(0).get<int>();
				stash.y = object.at("position").at(1).get<int>();

				auto tile = getTile(stash);

				if (!tile.isItemPickup())
					return JSONParserStatus::IncoherentData; // obv not everyhing is checkable at validator layer so eeeh

				stash_ = stash;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("dictionnaire",
			[this](json const & object)
			{
				Pos2D dict; // obviously should check A LOT OF STUFF (btw when i say check, i mean add a validator, that way everything is separate)
				dict.x = object.at("position").at(0).get<int>();
				dict.y = object.at("position").at(1).get<int>();

				auto tile = getTile(dict);

				if (!tile.isRecord())
					return JSONParserStatus::IncoherentData; // obv not everyhing is checkable at validator layer so eeeh

				dictionary_ = dict;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("residents",
			[this](json const & object)
			{
				for (auto const & nested : object)
				{
					Resident resident;

					resident.id_   = nested.at("id") .get<std::string>();
					resident.name_ = nested.at("nom").get<std::string>();


					resident.pos_.x = nested.at("position").at(0).get<int>();
					resident.pos_.y = nested.at("position").at(1).get<int>();

					auto tile = getTile(resident.pos_);

					if (!tile.isPerson())
						return JSONParserStatus::IncoherentData;

					residentList_.push_back(resident);
				}

				return JSONParserStatus::Ok;
			}
		)
	};
};
