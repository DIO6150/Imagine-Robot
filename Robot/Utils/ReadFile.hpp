#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>

inline std::optional<std::string> read_file(std::string path)
{
	std::ifstream file;
	file.open (path);

	if (!file.is_open ())
	{
		file.close ();

		std::cout << "Error: Couldn't open file " << path << "\n";

		return std::nullopt;
	}
	
	std::stringstream buffer;
	buffer << file.rdbuf();

	file.close();
	
	return buffer.str();
}

