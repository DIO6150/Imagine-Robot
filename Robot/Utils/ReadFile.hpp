#pragma once

#include <iostream>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <string>

#include <Robot/Errors/Error.hpp>
#include <vendor/nlohmann/json.hpp>

inline bool isValidUTF8(std::string dataToString)
{
	std::vector<std::byte> dataToBytes;
	for (char character : dataToString)
		dataToBytes.push_back(std::byte(character));

	std::byte one = std::byte{1};
	bool firstByteZero;
	bool secondByteZero;
	unsigned int nOtherCharBytes = 0;
	for (std::byte byte : dataToBytes)
	{
		secondByteZero = (((byte >> 6) & one) == one);
		if (nOtherCharBytes == 0)
		{
			for (int i = 7; (((byte >> i) & one) == one) && i > 3 ; i--)
				nOtherCharBytes++;

			if (nOtherCharBytes == 0)
				continue;
			
			if (nOtherCharBytes == 1 && secondByteZero)
				return false;

			if (nOtherCharBytes == 4 && (((byte >> 3) & one) == one))
				return false;
		}
		else
		{
			firstByteZero = (((byte >> 7) & one) == one);
			if (firstByteZero || (!firstByteZero && !secondByteZero))
				return false;
			nOtherCharBytes--;
		}
	}
	return true;
}

inline JSONParserStatus readJsonFile(std::string path, json & data)
{
	/*bool pathExists = std::filesystem::exists(path);
	if (!pathExists)
		return JSONParserStatus::IncorrectPath;

	std::ifstream file;
	file.open (path);

	if (!file.is_open ())
	{
		file.close();
		return JSONParserStatus::FileNotFound;
	}*/

	int fileAccess = open(path.c_str(), O_RDONLY);

	if (fileAccess == -1) {
        if (errno == ENOENT || errno == ENOTDIR)
            return JSONParserStatus::FileNotFound;
		return JSONParserStatus::IncorrectPath;
	}

	std::string stringData;
    char dataBuffer[4096];

	while (true) {
		ssize_t bytesRead = read(fileAccess, dataBuffer, sizeof(dataBuffer));

		if (bytesRead > 0)
			stringData.append(dataBuffer, static_cast<std::size_t>(bytesRead));
		else if (bytesRead == 0)
			break;
		else {
			close(fileAccess);
            	return JSONParserStatus::ReadingError;
		}
	}

	close(fileAccess);
	
	if(!isValidUTF8(stringData))
		return JSONParserStatus::NotUTF8;

	data = json::parse(stringData, nullptr, false);
	
	if(data.is_discarded())
		return JSONParserStatus::InvalidJson;

	return JSONParserStatus::Ok;
}

