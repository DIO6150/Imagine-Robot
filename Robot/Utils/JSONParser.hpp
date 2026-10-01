#pragma once

#include <functional>
#include <vector>
#include <string>

#include <vendor/nlohmann/json.hpp>

#include <Robot/Errors/Error.hpp>

using json = nlohmann::json;

class Field
{
public:
	using Validator     = std::function<bool(json const & object)>;
	using Filler        = std::function<JSONParserStatus(json const & object)>;
	using FillerNoError = std::function<void(json const & object)>;

	static Field validate(std::string name, Validator validator);
	static Field fill(std::string name, Filler filler);
	static Field fillNoError(std::string name, FillerNoError filler);
	static Field branch(std::string name, std::initializer_list<Field> children);
	
private:
	Field(std::string name);
	Field(std::string name, std::initializer_list<Field> children);

	std::string const name_;
	std::vector<Field> const children_;

	Validator validator_;
	Filler filler_;
	FillerNoError fillerNoError_;

	friend class JSONParser;
};

// TODO : check if requested field is the right type
class JSONParser
{
public:
	JSONParser(std::initializer_list<Field> const fields);
	JSONParserStatus fillFields(json const & object) const;

private:
	JSONParserStatus fillFields(json const & object, std::vector<Field> const & fields) const;
	JSONParserStatus checkFields(json const & object, std::vector<Field> const & fields) const;

private:
	std::vector<Field> const fields_;
};