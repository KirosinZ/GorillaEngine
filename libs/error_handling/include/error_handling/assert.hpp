#ifndef GORILLA_ASSERT_HPP
#define GORILLA_ASSERT_HPP

#include <string>
#include <source_location>


namespace gorilla::error_handling
{

void
asserts(
	bool expression,
	const std::string& message = "",
	const std::source_location& source = std::source_location::current());

}

#endif //GORILLA_ASSERT_HPP
