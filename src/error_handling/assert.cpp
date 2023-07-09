#include "assert.hpp"

#include <iostream>

#include <platform/build.hpp>

namespace error_handling
{

void
asserts(
		bool expression,
		const std::string& message,
		const std::source_location& source)
{
	if constexpr (build::type == build::build_type::debug)
	{
		if (!expression)
		{
			if (message.empty())
				std::cerr << "Assertion failed!\n";
			else
				std::cerr << "Assertion failed: " << message << "\n";

			std::cerr
					<< "File: " << source.file_name() << ":" << source.line() << ":" << source.column() << ", in function \"" << source.function_name() << "\""
					<< std::endl;

			std::abort();
		}
	}
}

}