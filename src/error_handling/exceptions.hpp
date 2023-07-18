#ifndef GORILLA_EXCEPTIONS_HPP
#define GORILLA_EXCEPTIONS_HPP

#include <stdexcept>
#include <concepts>

#include <boost/leaf/result.hpp>
#include <boost/leaf/error.hpp>
#include <boost/leaf/handle_errors.hpp>

namespace error_handling
{

using namespace boost::leaf;

}

namespace err = error_handling;

#endif //GORILLA_EXCEPTIONS_HPP
