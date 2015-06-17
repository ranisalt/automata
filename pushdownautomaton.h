#ifndef AUTOMATA_PUSHDOWNAUTOMATON_H
#define AUTOMATA_PUSHDOWNAUTOMATON_H

#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "exceptions.h"
#include "tools.h"

template<typename _Symbol = char, typename _State = std::string>
class PushdownAutomaton {
public:
	using Symbol = _Symbol;
	using State = _State;
	using Alphabet = std::unordered_set<Symbol>;
	using Input = std::tuple<State, Symbol, Symbol>;
	using Output = std::tuple<State, Symbol>;
	using TransitionMap = std::unordered_map<Input, Output>;
	using Transition = typename TransitionMap::value_type;
	using StateSet = std::unordered_set<State>;
	using invalid_symbol = exceptions::invalid_symbol<Symbol>;
	using invalid_state = exceptions::invalid_state<State>;

	PushdownAutomaton() = default;

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

	Alphabet stackAlphabet() const
	{ return _stack_alphabet; }

	template<typename... Args>
	void emplaceStackSymbol(Args&& ... args)
	{ _stack_alphabet.emplace(std::forward<Args>(args)...); }

	template<typename... Args>
	void insertStackSymbol(Args&& ... args)
	{ _stack_alphabet.insert(std::forward<Args>(args)...); }

	void insertStackSymbol(std::initializer_list<Symbol> ilist)
	{ insertStackSymbol(ilist.begin(), ilist.end()); }

	void removeStackSymbol(const Symbol& symbol);

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

	void insertTransition(const Input& input, const Output& output)
	{ insertTransition(std::make_pair(input, output)); }

	void insertTransition(const Transition& transition);

	void removeTransition(const Transition& transition);

	StateSet finalStates() const
	{ return _final_states; }

	void insertFinalState(const State& state);

	void removeFinalState(const State& state);

	const Symbol& epsilon() const
	{ return _epsilon; }

	template<typename InputIt>
	bool accept(const InputIt& first, const InputIt& last);

	bool accept(std::initializer_list<Symbol> ilist)
	{ return accept(ilist.begin(), ilist.end()); }

private:
	Alphabet _input_alphabet;
	Alphabet _stack_alphabet;
	StateSet _states;
	TransitionMap _transitions;
	State _initial_state;
	StateSet _final_states;
	Symbol _epsilon;
};

template<typename _Symbol, typename _State>
void PushdownAutomaton<_Symbol, _State>::removeSymbol(const Symbol& symbol)
{
	auto position = _input_alphabet.find(symbol);
	if (position == _input_alphabet.cend()) {
		throw invalid_symbol(symbol);
	}

	_input_alphabet.erase(position);
}

template<typename _Symbol, typename _State>
void PushdownAutomaton<_Symbol, _State>::removeStackSymbol(const Symbol& symbol)
{
	auto position = _stack_alphabet.find(symbol);
	if (position == _stack_alphabet.cend()) {
		throw invalid_symbol(symbol);
	}

	_stack_alphabet.erase(position);
}

template<typename _Symbol, typename _State>
void PushdownAutomaton<_Symbol, _State>::removeState(const State& state)
{
	auto position = _states.find(state);
	if (position == _states.cend()) {
		throw invalid_state(state);
	}

	_states.erase(position);
}

template<typename _Symbol, typename _State>
void PushdownAutomaton<_Symbol, _State>::initialState(const State& state)
{
	auto position = _states.find(state);
	if (position == _states.cend()) {
		throw invalid_state(state);
	}

	_initial_state = state;
}

template<typename _Symbol, typename _State>
void PushdownAutomaton<_Symbol, _State>::insertTransition(const Transition& transition)
{
	auto input = std::get<0>(transition);
	auto output = std::get<1>(transition);

	// input state must be valid
	if (_states.find(std::get<0>(input)) == _states.cend()) {
		throw invalid_state(std::get<0>(input));
	}

	// input symbol must be valid
	auto inputSymbol = std::get<1>(input);
	if (inputSymbol != epsilon() && _input_alphabet.find(inputSymbol) == _input_alphabet.cend()) {
		throw invalid_symbol(inputSymbol);
	}

	// input top stack symbol must be valid
	auto inputStackSymbol = std::get<2>(input);
	if (/*inputStackSymbol != epsilon() && */_stack_alphabet.find(inputStackSymbol) == _stack_alphabet.cend()) {
		throw invalid_symbol(inputStackSymbol);
	}

	// output state must be valid
	if (_states.find(std::get<0>(output)) == _states.cend()) {
		throw invalid_state(std::get<0>(output));
	}

	// output top stack symbol must be valid
	auto outputStackSymbol = std::get<1>(output);
	if (/*outputStackSymbol != epsilon() && */_stack_alphabet.find(outputStackSymbol) == _stack_alphabet.cend()) {
		throw invalid_symbol(outputStackSymbol);
	}

	_transitions.insert(transition);
}

template<typename _Symbol, typename _State>
void PushdownAutomaton<_Symbol, _State>::insertFinalState(const State& state)
{
	auto position = _states.find(state);
	if (position == _states.cend()) {
		throw invalid_state(state);
	}

	_final_states.insert(state);
}

template<typename _Symbol, typename _State>
void PushdownAutomaton<_Symbol, _State>::removeFinalState(const State& state)
{
	auto position = _final_states.find(state);
	if (position == _final_states.cend()) {
		throw invalid_state(state);
	}

	_final_states.erase(position);
}

template<typename _Symbol, typename _State>
template<typename InputIt>
bool PushdownAutomaton<_Symbol, _State>::accept(const InputIt& first, const InputIt& last)
{
	auto currentState = _initial_state;
	auto stack = std::stack<Symbol>();

	for (auto it = first; it != last; ++it) {
		auto stackTop = stack.empty() ? epsilon() : stack.top();

		auto position = _transitions.find(std::make_tuple(currentState, *it, stackTop));
		if (position == _transitions.cend()) {
			position = _transitions.find(std::make_tuple(currentState, epsilon(), stackTop));
			if (position == _transitions.cend()) {
				return false;
			}
		}
		if (stackTop != epsilon()) {
			stack.pop();
		}

		currentState = std::get<0>(position->second);
		auto stackSymbol = std::get<1>(position->second);
		if (stackSymbol != epsilon()) {
			stack.push(stackSymbol);
		}
	}

	return true;
}

#endif //AUTOMATA_PUSHDOWNAUTOMATON_H
