
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

JSONParser::JSONParser(std::initializer_list<std::string> const fieldsNames, std::initializer_list<Field> const fields)
	: fieldsNames_{fieldsNames}
	, fields_{fields}
{

}

JSONParserStatus JSONParser::verifySimpleString(json const & object)
{ 
	if (!object.is_string())
		return JSONParserStatus::WrongType;
	return JSONParserStatus::Ok;
}

JSONParserStatus JSONParser::verifySimplePos(json const & object)
{
	if (!object.is_array() && !object.is_object())
		return JSONParserStatus::WrongType;

	if (object.is_object() && !object.contains("position"))
		return JSONParserStatus::MissingField;

	json const & array = ((object.is_array()) ? object : object["position"]);

	if (array.size() != 2)
		return JSONParserStatus::PositionWrongSize;
	
	if (!array.at(0).is_number_integer() || !array.at(1).is_number_integer())
		return JSONParserStatus::WrongType;

	return JSONParserStatus::Ok;
}

JSONParserStatus JSONParser::checkFields(json const & object) const
{
	for (std::string fieldName : fieldsNames_)
	{
		if (!object.contains(fieldName))
			return JSONParserStatus::MissingField;
	}
	
	return checkFields(object, fields_);
}

JSONParserStatus JSONParser::checkFields(nlohmann::json const & object, std::vector<Field> const & fields) const
{
	JSONParserStatus status = JSONParserStatus::Ok;
	
	for (auto const & field : fields)
	{
		auto const & currentObject = object.at(field.name_);

		if (field.validator_)
		{
			status = field.validator_(currentObject);
			if (status != JSONParserStatus::Ok) return status;
		}

		if(!field.children_.empty())
		{
			if(currentObject.is_array())
			{
				for(auto const & arrayElem : currentObject)
				{
					status = checkFields(arrayElem, field.children_);
					if (status != JSONParserStatus::Ok) return status;
				}
			}
			else
			{
				status = checkFields(currentObject, field.children_);
				if (status != JSONParserStatus::Ok) return status;
			}
		}

		
	}

	return status;
}

JSONParserStatus JSONParser::fillFields(json const & object) const
{
	JSONParserStatus status = checkFields(object);
	if(status != JSONParserStatus::Ok)
		return status;
	
	return fillFields(object, fields_);
}

JSONParserStatus JSONParser::fillFields(json const & object, std::vector<Field> const & fields) const
{
	JSONParserStatus status = JSONParserStatus::Ok;

	for (auto field : fields)
	{
		auto const & currentObject = object.at(field.name_);

		if (field.filler_)
		{
			status = field.filler_(currentObject);
			if (status != JSONParserStatus::Ok) return status;
		}

		if (field.fillerNoError_)
			field.fillerNoError_(currentObject);
	}
	return status;
}
