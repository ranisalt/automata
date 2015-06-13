#ifndef AUTOMATA_EXCEPTIONS_H
#define AUTOMATA_EXCEPTIONS_H

#include <exception>

namespace exceptions {

// Exception for invalid symbol
template<typename Symbol>
class invalid_symbol : public std::invalid_argument {
public:
	invalid_symbol(const Symbol& symbol) : std::invalid_argument("Invalid symbol"), _symbol(symbol)
	{ }

	virtual const char* what() const throw()
	{
		std::ostringstream oss;
		oss << std::invalid_argument::what() << ": " << _symbol;
		return oss.str().c_str();
	}

private:
	Symbol _symbol;
};

// Exception for invalid state usage
template<typename State>
class invalid_state : public std::invalid_argument {
public:
	invalid_state(const State& state) : std::invalid_argument("Invalid state"), _state(state)
	{ }

	virtual const char* what() const throw()
	{
		std::ostringstream oss;
		oss << std::invalid_argument::what() << ": " << _state;
		return oss.str().c_str();
	}

private:
	State _state;
};

}

#endif //AUTOMATA_EXCEPTIONS_H
