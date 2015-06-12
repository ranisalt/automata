#include "finitestateautomaton.h"

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
	std::pair<State, Symbol> input = transition.first;
	State output = transition.second;

	// ensure input state is valid
	if (_states.find(input.first) == _states.cend()) {
		throw invalid_state(input.first);
	}

	// ensure input symbol is valid
	if (_input_alphabet.find(input.second) == _input_alphabet.cend()) {
		throw invalid_symbol(input.second);
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
bool FiniteStateAutomaton<_Symbol, _State>::accept(const InputIt& first, const InputIt& last)
{
	auto current_state = _initial_state;

	for (auto it = first; it != last; ++it) {
		auto position = _transitions.find(std::make_pair(current_state, *it));
		if (position == _transitions.cend()) {
			return false;
		}

		current_state = position->second;
	}

	auto position = _final_states.find(current_state);
	return position != _final_states.cend();
}
