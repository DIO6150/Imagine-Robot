
#include <Robot/Utils/JSONParser.hpp>

Field::Field(std::string name)
	: name_{name}
{

}

Field::Field(std::string name, std::initializer_list<Field> children)
	: name_{name}
	, children_{children}
{

}

Field Field::validate(std::string name, Validator validator)
{
	Field field {name};
	field.validator_ = validator;
	return field;
}

Field Field::fill(std::string name, Filler filler)
{
	Field field {name};
	field.filler_ = filler;
	return field;
}

Field Field::fillNoError(std::string name, FillerNoError filler)
{
	Field field {name};
	field.fillerNoError_ = filler;
	return field;
}

Field Field::branch(std::string name, std::initializer_list<Field> children)
{
	Field field {name, children};
	return field;
}

JSONParser::JSONParser(std::initializer_list<Field> const fields)
	: fields_{fields}
{

}

JSONParserStatus JSONParser::fillFields(json const & object) const
{
	auto result = checkFields(object, fields_);

	if ((int)result)
		return result;

	result = fillFields(object, fields_);

	return result;
}

JSONParserStatus JSONParser::fillFields(json const & object, std::vector<Field> const & fields) const
{
	for (auto field : fields)
	{
		auto const & currentObject = object.at(field.name_);

		if (field.filler_)
		{
			auto status = field.filler_(currentObject);

			if ((int)status) return status;
		}

		if (field.fillerNoError_)
		{
			field.fillerNoError_(currentObject);
		}

		if (!field.children_.empty())
		{
			auto status = fillFields(currentObject, field.children_);
			if ((int)status) return status;
		}
	}

	return JSONParserStatus::Ok;
}

JSONParserStatus JSONParser::checkFields(nlohmann::json const & object, std::vector<Field> const & fields) const
{
	for (auto const & field : fields)
	{
		if (!object.contains(field.name_))
			return JSONParserStatus::MissingField;

		auto const & currentObject = object.at(field.name_);

		if (field.validator_)
		{
			if (!field.validator_(currentObject))
				return JSONParserStatus::WrongValue;
		}

		if (!field.children_.empty())
		{
			auto result = checkFields(currentObject, field.children_);
			if (!(int)result) return result;
		}
	}

	return JSONParserStatus::Ok;
}
