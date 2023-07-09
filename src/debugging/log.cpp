#include "log.hpp"

#include <string>
#include <format>

namespace debug
{

log::log(std::streambuf* output)
	: m_output(*output)
{}

log_session log::info()
{
	return log_session(*this, message_category::info);
}

log_session log::warn()
{
	return log_session(*this, message_category::warn);
}

log_session log::error()
{
	return log_session(*this, message_category::error);
}

}