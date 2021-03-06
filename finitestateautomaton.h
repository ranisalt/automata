#ifndef AUTOMATA_FINITESTATEAUTOMATON_H
#define AUTOMATA_FINITESTATEAUTOMATON_H

#include <experimental/optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "exceptions.h"
#include "tools.h"

template<typename _Symbol = char, typename _State = std::string>
class FiniteStateAutomaton {
public:
	using Symbol = _Symbol;
	using State = _State;
	using Alphabet = std::unordered_set<Symbol>;
	using Input = std::tuple<State, std::experimental::optional<Symbol>>;

	struct KeyEqual {
		bool operator()(const Input& lhs, const Input& rhs) const
		{
			if (std::get<0>(lhs) != std::get<0>(rhs)) {
				return false;
			}

			auto lval = std::get<1>(lhs), rval = std::get<1>(rhs);
			if (lval && rval && lval.value() != rval.value()) {
				return false;
			}
			return true;
		};
	};

	using TransitionMap = std::unordered_multimap<Input, State, std::hash<Input>, KeyEqual>;
	using Transition = typename TransitionMap::value_type;
	using StateSet = std::unordered_set<State>;
	using invalid_symbol = exceptions::invalid_symbol<Symbol>;
	using invalid_state = exceptions::invalid_state<State>;

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

	void insertTransition(const Input& input, const State& output)
	{ insertTransition(std::make_pair(input, output)); }

	void insertTransition(const Transition& transition);

	void removeTransition(const Transition& transition);

	StateSet finalStates() const
	{ return _final_states; }

	void insertFinalState(const State& state);

	void removeFinalState(const State& state);

	template<typename Container>
	bool accept(const Container& sentence) const
	{ return internalAccept(_initial_state, sentence.cbegin(), sentence.cend()); }

private:
	template<typename InputIt>
	bool internalAccept(const State& current_state, const InputIt& first, const InputIt& last) const;

	// Automaton configuration
	Alphabet _input_alphabet;
	StateSet _states;
	State _initial_state;
	TransitionMap _transitions;
	StateSet _final_states;
};

template<typename _Symbol, typename _State>
FiniteStateAutomaton<_Symbol, _State>::FiniteStateAutomaton(const FiniteStateAutomaton::Alphabet& input_alphabet,
	const FiniteStateAutomaton::StateSet& states) : _input_alphabet(input_alphabet), _states(states),
	_initial_state(states.cend()), _transitions(), _final_states()
{ }

template<typename _Symbol, typename _State>
FiniteStateAutomaton<_Symbol, _State>::FiniteStateAutomaton(const Alphabet& input_alphabet, const StateSet& states,
	const State& initial_state, const TransitionMap& transitions, const StateSet& final_states) :
	_input_alphabet(input_alphabet), _states(states), _initial_state(initial_state), _transitions(transitions),
	_final_states(final_states)
{
	// initial state must be a valid state
	if (states.find(initial_state) == states.cend()) {
		throw 1;
	}

	// transitions must originate from a valid state, consume a valid character and move to a valid state
	for (const auto& transition : transitions) {
		if (states.find(transition.first.first) == states.cend() ||
			input_alphabet.find(transition.first.second) != input_alphabet.cend() ||
			states.find(transition.second) != states.cend()) {
			throw 1;
		}
	}

	// final states must be valid states
	for (const auto& state : final_states) {
		if (states.find(state) == states.cend()) {
			throw 1;
		}
	}
}

template<typename _Symbol, typename _State>
void FiniteStateAutomaton<_Symbol, _State>::removeSymbol(const Symbol& symbol)
{
	auto position = _input_alphabet.find(symbol);
	if (position == _input_alphabet.cend()) {
		throw invalid_symbol(symbol);
	}

	_input_alphabet.erase(position);
}

template<typename _Symbol, typename _State>
void FiniteStateAutomaton<_Symbol, _State>::removeState(const State& state)
{
	auto position = _states.find(state);
	if (position == _states.cend()) {
		throw invalid_state(state);
	}

	_states.erase(position);
}

template<typename _Symbol, typename _State>
void FiniteStateAutomaton<_Symbol, _State>::initialState(const State& state)
{
	auto position = _states.find(state);
	if (position == _states.cend()) {
		throw invalid_state(state);
	}

	_initial_state = state;
}

template<typename _Symbol, typename _State>
void FiniteStateAutomaton<_Symbol, _State>::insertTransition(const Transition& transition)
{
	Input input = std::get<0>(transition);
	State output = std::get<1>(transition);

	// ensure input state is valid
	if (_states.find(std::get<0>(input)) == _states.cend()) {
		throw invalid_state(std::get<0>(input));
	}

	// ensure input symbol is valid
	auto symbol = std::get<1>(input);
	if (symbol) {
		auto val = symbol.value();
		if (_input_alphabet.find(val) == _input_alphabet.cend()) {
			throw invalid_symbol(val);
		}
	}

	// ensure output state is valid
	if (_states.find(output) == _states.cend()) {
		throw invalid_state(output);
	}

	_transitions.insert(transition);
}

template<typename _Symbol, typename _State>
void FiniteStateAutomaton<_Symbol, _State>::removeTransition(const Transition& transition)
{
	auto position = _transitions.find(transition.first);
	if (position == _transitions.cend() || position->second != transition.second) {
		return;
	}

	_transitions.erase(position);
}

template<typename _Symbol, typename _State>
void FiniteStateAutomaton<_Symbol, _State>::insertFinalState(const State& state)
{
	if (_states.find(state) == _states.cend()) {
		throw invalid_state(state);
	}

	_final_states.insert(state);
}

template<typename _Symbol, typename _State>
void FiniteStateAutomaton<_Symbol, _State>::removeFinalState(const State& state)
{
	auto position = _states.find(state);
	if (position == _states.cend()) {
		throw invalid_state(state);
	}

	_final_states.erase(position);
}

template<typename _Symbol, typename _State>
template<typename InputIt>
bool FiniteStateAutomaton<_Symbol, _State>::internalAccept(const State& current_state, const InputIt& first,
	const InputIt& last) const
{
	if (first == last) {
		return _final_states.find(current_state) != _final_states.cend();
	}

	auto range = _transitions.equal_range(std::make_pair(current_state, *first));
	if (range.first == range.second) {
		return false;
	}

	for (auto it = range.first; it != range.second; ++it) {
		if (internalAccept(it->second, std::next(first), last)) {
			return true;
		}
	}
	return false;
};

#endif //AUTOMATA_FINITESTATEAUTOMATON_H
