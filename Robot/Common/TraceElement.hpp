#pragma once

#include <Robot/Common/Emotion.hpp>
#include <Robot/Common/Orientation.hpp>
#include <Robot/Common/Tile.hpp>
#include <Robot/Utils/Pos.hpp>

#include <map>

struct Pas
{
	uint32_t t;
	uint32_t demande;
	Pos2D position;
	Pos2D casier;
	std::string action;
	std::string argument;
    std::map<std::string, std::string> perception;
    std::string contenu_casier;
    std::string commentaire;
};

struct Livraison
{
	uint32_t demande;
	std::string resident;
	std::string emotion;
	std::string intensity;
	Pos2D casier_choisi;
	std::string repli;
    std::string objet;
    uint32_t pas_utilises;
    bool succes;
    std::string motif_echec;
};