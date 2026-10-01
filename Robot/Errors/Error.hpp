#pragma once

enum class Error
{
	Success,
	JSONSyntaxError
};

enum class JSONParserStatus: int
{
	Ok                   = 0,
	MissingField         = 1,
	WrongType            = 2,
	WrongValue           = 3,
	UnknownProperty      = 4,
	IncoherentData       = 5,
};
