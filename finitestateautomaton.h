#ifndef AUTOMATA_FINITESTATEAUTOMATON_H
#define AUTOMATA_FINITESTATEAUTOMATON_H

#include <exception>
#include <forward_list>
#include <string>
#include <unordered_map>
#include <unordered_set>

template<typename _Symbol = char, typename _State = std::string>
class FiniteStateAutomaton {
	// Specialized hash function for pairs, needed for maps and sets. From Boost library.
	template<typename T1, typename T2>
	struct PairHash {
		size_t operator()(const std::pair<T1, T2>& pair) const
		{
			size_t seed = 0;
			seed ^= std::hash<T1>()(pair.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= std::hash<T2>()(pair.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};

public:
	using Symbol = _Symbol;
	using State = _State;
	using Alphabet = std::unordered_set<Symbol>;
	using Input = std::pair<State, Symbol>;
	using TransitionMap = std::unordered_map<Input, State, PairHash<State, Symbol>>;
	using Transition = typename TransitionMap::value_type;
	using StateSet = std::unordered_set<State>;

	// Exception for invalid symbol usage
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

	// Initialize empty automaton
	// TODO: rethink, since it makes the machine get to an invalid state (no data at all)
	FiniteStateAutomaton() = default;

	// Initialize automaton with alphabet and states
	// TODO: rethink, since it makes the machine get to an invalid state (no initial state)
	FiniteStateAutomaton(const Alphabet& input_alphabet, const StateSet& states);

	// Initialize complete automaton
	// TODO: rethink, since it is a throwing ctor (checks for machine validity)
	FiniteStateAutomaton(const Alphabet& input_alphabet, const StateSet& states, const State& initial_state,
		const TransitionMap& transitions, const StateSet& final_states);

	Alphabet alphabet() const
	{ return _input_alphabet; }

	template<typename... Args>
	void emplaceSymbol(Args&& ... args)
	{ _input_alphabet.emplace(std::forward<Args>(args)...); }

	template<typename... Args>
	void insertSymbol(Args&& ... args)
	{ _input_alphabet.insert(std::forward<Args>(args)...); }

	void insertSymbol(std::initializer_list<Symbol> ilist)
	{ insertSymbol(ilist.begin(), ilist.end()); }

	void removeSymbol(const Symbol& symbol);

	StateSet states() const
	{ return _states; }

	template<typename... Args>
	void emplaceState(Args&& ... args)
	{ _states.emplace(std::forward<Args>(args)...); }

	template<typename... Args>
	void insertState(Args&& ... args)
	{ _states.insert(std::forward<Args>(args)...); }

	void insertState(std::initializer_list<State> ilist)
	{ insertState(ilist.begin(), ilist.end()); }

	void removeState(const State& state);

	State initialState() const
	{ return _initial_state; }

	void initialState(const State& state);

	TransitionMap transitions() const
	{ return _transitions; }

	void insertTransition(const Input& input, const State output)
	{ insertTransition(std::make_pair(input, output)); }

	void insertTransition(const Transition& transition);

	void removeTransition(const Transition& transition);

	StateSet finalStates() const
	{ return _final_states; }

	void insertFinalState(const State& state);

	void removeFinalState(const State& state);

	template<typename InputIt>
	bool accept(const InputIt& first, const InputIt& last);

	bool accept(std::initializer_list<Symbol> ilist)
	{ return accept(ilist.begin(), ilist.end()); }

private:
	// Automaton configuration
	Alphabet _input_alphabet;
	StateSet _states;
	State _initial_state;
	TransitionMap _transitions;
	StateSet _final_states;
};

#include "finitestateautomaton.cpp"

#endif //AUTOMATA_FINITESTATEAUTOMATON_H
