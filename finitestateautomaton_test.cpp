#include <array>
#include <experimental/optional>
#include <gtest/gtest.h>
#include "exceptions.h"
#include "finitestateautomaton.h"

class FiniteStateAutomaton_test : public testing::Test {
public:
	using FSM = FiniteStateAutomaton<char, std::string>;
	FSM fsm;
};

TEST_F(FiniteStateAutomaton_test, isCreatedEmpty)
{
	EXPECT_EQ(0, fsm.alphabet().size()) << "Alphabet is not created empty";
	EXPECT_EQ(0, fsm.states().size()) << "State set is not created empty";
	EXPECT_EQ(0, fsm.transitions().size()) << "Transition map is not created empty";
	EXPECT_EQ(0, fsm.finalStates().size()) << "Final states set is not created empty";
}

TEST_F(FiniteStateAutomaton_test, insertSymbol)
{
	fsm.insertSymbol('a');
	EXPECT_EQ(1, fsm.alphabet().size()) << "Insert single symbol does not work";

	std::array<char, 2> arr = {'b', 'c'};
	fsm.insertSymbol(arr.cbegin(), arr.cend());
	EXPECT_EQ(3, fsm.alphabet().size()) << "Insert symbols with iterators does not work";

	fsm.insertSymbol({'d', 'e'});
	EXPECT_EQ(5, fsm.alphabet().size()) << "Insert symbols with initializer list does not work";

	fsm.emplaceSymbol('f');
	EXPECT_EQ(6, fsm.alphabet().size()) << "Emplace single symbol does not work";
}

TEST_F(FiniteStateAutomaton_test, insertDuplicatedSymbol)
{
	// initial symbol
	fsm.insertSymbol('a');

	fsm.insertSymbol('a');
	EXPECT_EQ(1, fsm.alphabet().size()) << "Insert single symbol creates duplicates";

	std::array<char, 2> ilist = {'a', 'b'};
	fsm.insertSymbol(ilist.cbegin(), ilist.cend());
	EXPECT_EQ(2, fsm.alphabet().size()) << "Insert symbols with iterators creates duplicates";

	fsm.insertSymbol({'a', 'b', 'c'});
	EXPECT_EQ(3, fsm.alphabet().size()) << "Insert symbols with initializer list creates duplicates";

	fsm.emplaceSymbol('a');
	EXPECT_EQ(3, fsm.alphabet().size()) << "Emplace single symbol creates duplicates";
}

TEST_F(FiniteStateAutomaton_test, removeSymbol)
{
	fsm.insertSymbol({'a', 'b'});

	EXPECT_NO_THROW(fsm.removeSymbol('a'));
	EXPECT_EQ(FSM::Alphabet{'b'}, fsm.alphabet());

	EXPECT_THROW(fsm.removeSymbol('a'), FSM::invalid_symbol);
}


TEST_F(FiniteStateAutomaton_test, insertState)
{
	fsm.insertState("q0");
	EXPECT_EQ(1, fsm.states().size()) << "Insert single symbol does not work";

	std::array<std::string, 2> arr = {"q1", "q2"};
	fsm.insertState(arr.cbegin(), arr.cend());
	EXPECT_EQ(3, fsm.states().size()) << "Insert states with iterators does not work";

	fsm.insertState({"q3", "q4"});
	EXPECT_EQ(5, fsm.states().size()) << "Insert states with initializer list does not work";

	fsm.emplaceState("q5");
	EXPECT_EQ(6, fsm.states().size()) << "Emplace single state does not work";
}

TEST_F(FiniteStateAutomaton_test, insertDuplicatedState)
{
	fsm.insertState("q0");
	EXPECT_EQ(1, fsm.states().size()) << "Insert single symbol creates duplicates";

	std::array<std::string, 2> arr = {"q0", "q1"};
	fsm.insertState(arr.cbegin(), arr.cend());
	EXPECT_EQ(2, fsm.states().size()) << "Insert states with iterators creates duplicates";

	fsm.insertState({"q0", "q1", "q2"});
	EXPECT_EQ(3, fsm.states().size()) << "Insert states with initializer list creates duplicates";

	fsm.emplaceState("q0");
	EXPECT_EQ(3, fsm.states().size()) << "Emplace single state creates duplicates";
}

TEST_F(FiniteStateAutomaton_test, removeState)
{
	fsm.insertState({"q0", "q1"});

	EXPECT_NO_THROW(fsm.removeState("q0"));
	EXPECT_EQ(FSM::StateSet{"q1"}, fsm.states());

	// q0 is not there anymore
	EXPECT_THROW(fsm.removeState("q0"), FSM::invalid_state);
}

TEST_F(FiniteStateAutomaton_test, setInitialState)
{
	EXPECT_THROW(fsm.initialState("q0"), FSM::invalid_state);

	fsm.insertState("q0");
	EXPECT_NO_THROW(fsm.initialState("q0"));
	EXPECT_EQ("q0", fsm.initialState());
}

TEST_F(FiniteStateAutomaton_test, insertTransition)
{
	fsm.insertSymbol({'a'});
	fsm.insertState({"q0", "q1"});

	FSM::TransitionMap map;

	EXPECT_NO_THROW(fsm.insertTransition(std::make_tuple("q0", 'a'), "q1"));
	map.emplace(FSM::Input("q0", 'a'), "q1");
	EXPECT_EQ(map, fsm.transitions());

	EXPECT_NO_THROW(fsm.insertTransition({std::make_tuple("q1", std::experimental::nullopt), "q0"}));
	map.emplace(FSM::Input("q1", std::experimental::nullopt), "q0");
	EXPECT_EQ(map, fsm.transitions());
}

TEST_F(FiniteStateAutomaton_test, insertInvalidTransition)
{
	// machine is empty
	EXPECT_THROW(fsm.insertTransition(std::make_tuple("q0", 'a'), "q1"), FSM::invalid_state);
	EXPECT_EQ(0, fsm.transitions().size());

	// "q0" exists, but 'a' is not a valid symbol
	fsm.insertState("q0");
	EXPECT_THROW(fsm.insertTransition(std::make_tuple("q0", 'a'), "q1"), FSM::invalid_symbol);
	EXPECT_EQ(0, fsm.transitions().size());

	// "q0" and 'a' exists, but "q1" is not a valid state
	fsm.insertSymbol('a');
	EXPECT_THROW(fsm.insertTransition(std::make_tuple("q0", 'a'), "q1"), FSM::invalid_state);
	EXPECT_EQ(0, fsm.transitions().size());
}

TEST_F(FiniteStateAutomaton_test, insertFinalState)
{
	fsm.insertState("q0");

	EXPECT_NO_THROW(fsm.insertFinalState("q0"));
	EXPECT_EQ(FSM::StateSet{"q0"}, fsm.finalStates());
}

TEST_F(FiniteStateAutomaton_test, insertInvalidFinalState)
{
	EXPECT_THROW(fsm.insertFinalState("q0"), FSM::invalid_state);
	EXPECT_EQ(0, fsm.finalStates().size());
}

TEST_F(FiniteStateAutomaton_test, acceptAndReject)
{
	// alphabet = {a, b}
	// language = { (a|b)*b }
	// image: http://www.csd.uwo.ca/~moreno//CS447/Lectures/Lexical.html/img17.png
	fsm.insertSymbol({'a', 'b'});
	fsm.insertState({"q0", "q1", "q2"});
	fsm.initialState("q0");
	fsm.insertTransition(std::make_tuple("q0", 'a'), "q1");
	fsm.insertTransition(std::make_tuple("q0", 'b'), "q2");
	fsm.insertTransition(std::make_tuple("q1", 'a'), "q1");
	fsm.insertTransition(std::make_tuple("q1", 'b'), "q2");
	fsm.insertTransition(std::make_tuple("q2", 'a'), "q1");
	fsm.insertTransition(std::make_tuple("q2", 'b'), "q2");
	fsm.insertFinalState("q2");

	EXPECT_TRUE(fsm.accept({'a', 'a', 'b'}));
	EXPECT_FALSE(fsm.accept({'b', 'a', 'a'}));
}
