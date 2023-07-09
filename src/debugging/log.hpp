#ifndef GORILLA_LOG_HPP
#define GORILLA_LOG_HPP

#include <iostream>
#include <functional>

namespace debug
{

enum class message_category
{
	info,
	warn,
	error
};

class log_session;

class log
{
public:
	explicit log(std::streambuf* output);

	log_session info();
	log_session warn();
	log_session error();

	inline std::streambuf& output() const { return m_output; }
	inline const std::vector<std::string>& entries() const { return m_entries; }
	inline std::vector<std::string>& entries() { return m_entries; }
private:
	std::streambuf& m_output;
	std::vector<std::string> m_entries;
};

class log_session : public std::ostream
{
	friend class log;
public:

	inline ~log_session() override
	{
		*this << std::endl;
	}

private:
	inline log_session(log& parent, message_category severity)
		: std::ostream(&parent.output()),
		m_parent(parent)
	{
		*this << "[Msg]: ";
	}

	log& m_parent;
};



}

#endif //GORILLA_LOG_HPP
