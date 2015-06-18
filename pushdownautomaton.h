#ifndef AUTOMATA_PUSHDOWNAUTOMATON_H
#define AUTOMATA_PUSHDOWNAUTOMATON_H

#include <algorithm>
#include <deque>
#include <experimental/optional>
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
    using Input = std::tuple<State, std::experimental::optional<Symbol>, std::experimental::optional<Symbol>>;
    using Output = std::tuple<State, std::deque<Symbol>>;

    struct KeyEqual {
        bool operator()(const Input& lhs, const Input& rhs) const
        {
            if (std::get<0>(lhs) != std::get<0>(rhs)) {
                return false;
            }

            {
                auto lval = std::get<1>(lhs), rval = std::get<1>(rhs);
                if (lval && rval && lval.value() != rval.value()) {
                    return false;
                }
            }

            {
                auto lval = std::get<2>(lhs), rval = std::get<2>(rhs);
                if (lval && rval && lval.value() != rval.value()) {
                    return false;
                }
            }
            return true;
        };
    };

    using TransitionMap = std::unordered_map<Input, Output, std::hash<Input>, KeyEqual>;
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

    template<typename InputIt>
    bool accept(const InputIt& first, const InputIt& last);

    template<typename Container>
    std::pair<bool, std::stack<Symbol>> accept(const Container& sentence) const
    {
        std::stack<Symbol> stack;
        return {internalAccept(_initial_state, sentence.cbegin(), sentence.cend(), stack), stack};
    }

    static constexpr auto& epsilon = std::experimental::nullopt;

private:
    template<typename InputIt>
    std::pair<bool, std::stack<Symbol>> internalAccept(const State& current_state, const InputIt& first,
            const InputIt& last, std::stack<Symbol>& stack) const;

    Alphabet _input_alphabet;
    Alphabet _stack_alphabet;
    StateSet _states;
    TransitionMap _transitions;
    State _initial_state;
    StateSet _final_states;
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
    if (inputSymbol && _input_alphabet.find(inputSymbol.value()) == _input_alphabet.cend()) {
        throw invalid_symbol(inputSymbol.value());
    }

    // input top stack symbol must be valid
    auto inputStackSymbol = std::get<2>(input);
    if (inputStackSymbol && _stack_alphabet.find(inputStackSymbol.value()) == _stack_alphabet.cend()) {
        throw invalid_symbol(inputStackSymbol.value());
    }

    // output state must be valid
    if (_states.find(std::get<0>(output)) == _states.cend()) {
        throw invalid_state(std::get<0>(output));
    }

    // output top stack symbol must be valid
    auto outputStackSymbols = std::get<1>(output);
    if (!outputStackSymbols.empty()) {
        for (const auto& symbol : outputStackSymbols) {
            if (_stack_alphabet.find(symbol) == _stack_alphabet.cend()) {
                throw invalid_symbol(symbol);
            }
        }
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
std::pair<bool, std::stack<_Symbol>>  PushdownAutomaton<_Symbol, _State>::internalAccept(const State& current_state,
        const InputIt& first, const InputIt& last, std::stack<_Symbol>& stack) const
{
    using std::experimental::nullopt;
    using std::experimental::optional;

    if (first == last) {
        return {_final_states.find(current_state) != _final_states.cend(), stack};
    }

    optional<Symbol> stackTop;
    if (!stack.empty()) {
        stackTop = stack.top();
    }

    auto range = _transitions.equal_range(std::make_tuple(current_state, *first, stackTop));
    if (range.first == range.second) {
        return {false, stack};
    }

    for (auto it = range.first; it != range.second; ++it) {
        std::stack<Symbol> next_stack{stack};

        auto symbolsToPush = std::get<1>(*it);
        for (auto rit = symbolsToPush.rbegin(); rit != symbolsToPush.rend(); ++rit) {
            next_stack.push(*rit);
        }

        if (auto ret = internalAccept(it->second, std::next(first), last, next_stack)) {
            return ret;
        }
    };

    /*current_tate = std::get<0>(position->second);
    auto stackSymbol = std::get<1>(position->second);
    if (stackSymbol != epsilon()) {
        stack.push(stackSymbol);
    }*/


    return {false, stack};
}

#endif //AUTOMATA_PUSHDOWNAUTOMATON_H
