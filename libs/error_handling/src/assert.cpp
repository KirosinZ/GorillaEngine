#include "error_handling/assert.hpp"

#include <iostream>


namespace gorilla::error_handling
{

void
asserts(
	bool expression,
	const std::string& message,
	const std::source_location& source)
{
	if (expression)
		return;

	if (message.empty())
		std::cerr << "Assertion failed!\n";
	else
		std::cerr << "Assertion failed: " << message << "\n";

	std::cerr
		<< "File: "
		<< source.file_name()
		<< ":"
		<< source.line()
		<< ":"
		<< source.column()
		<< ", in function \""
		<< source.function_name()
		<< "\""
		<< std::endl;

	std::abort();
}

}