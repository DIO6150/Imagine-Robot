#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>
#include <optional>

#include <Robot/Errors/Error.hpp>
#include <vendor/nlohmann/json.hpp>

inline bool isValidUTF8(json data)
{
	std::string dataToString = data.dump();
	std::vector<std::byte> dataToBytes;
	for (unsigned char character : dataToString)
		dataToBytes.push_back(character);

	bool firstByteZero;
	bool secondByteZero;
	unsigned int nOtherCharBytes = 0;
	for (std::byte byte : dataToBytes)
	{
		secondByteZero = (((byte >> 6) & 1) == 1);
		if (nOtherCharBytes == 0)
		{
			for (int i = 7; (((byte >> i) & 1) == 1) && i > 3 ; i--)
				nOtherCharBytes++;

			if (nOtherCharBytes == 0)
				continue;
			
			if (nOtherCharBytes == 1 && secondByteZero)
				return false;

			if (nOtherCharBytes == 4 && (((byte >> 3) & 1) == 1))
				return false;
		}
		else
		{
			firstByteZero = (((byte >> 7) & 1) == 1);
			if (firstByteZero || (!firstByteZero && !secondByteZero))
				return false;
			nOtherCharBytes--;
		}
	}
}

inline JSONParserStatus readJsonFile(std::string path, json & data)
{
	bool pathExists = std::filesystem::exists(path(path));
	if (!pathExists)
		return JSONParserStatus::IncorrectPath;

	std::ifstream file;
	file.open (path);

	if (!file.is_open ())
	{
		file.close();
		return JSONParserStatus::FileNotFound;
	}

	file.close();
	
	if (!readData)
		return JSONParserStatus::FileNotFound;

	data = json::parse(file, nullptr, false);

	if(data.is_discarded())
		return JSONParserStatus::InvalidJson;
	
	if(!isValidUTF8(data))
		return JSONParserStatus::NotUTF8;

	return JSONParserStatus::Ok;
}

