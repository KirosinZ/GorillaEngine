#ifndef GORILLA_ASSERT_HPP
#define GORILLA_ASSERT_HPP

#include <string>
#include <source_location>

namespace error_handling
{

void
asserts(
		bool expression,
		const std::string& message = "",
		const std::source_location& source = std::source_location::current());

}

namespace err = error_handling;

#endif //GORILLA_ASSERT_HPP
