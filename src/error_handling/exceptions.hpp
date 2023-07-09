#ifndef GORILLA_EXCEPTIONS_HPP
#define GORILLA_EXCEPTIONS_HPP

#include <stdexcept>
#include <concepts>

namespace error_handling
{
template<typename T>
concept exception_c = std::derived_from<T, std::exception>;


template<exception_c E = std::runtime_error>
inline void
throw_if(bool check, const std::string& message = "");

template<exception_c E = std::invalid_argument>
inline void
validate_arg(bool arg_check, const std::string& message = "");






template<exception_c E>
void
throw_if(bool check, const std::string& message)
{
	if (check)
		throw E(message);
}

template<exception_c E>
void
validate_arg(bool arg_check, const std::string& message)
{
	throw_if<E>(!arg_check, message);
}

}

#endif //GORILLA_EXCEPTIONS_HPP
