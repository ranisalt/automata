#include <gtest/gtest.h>
#include "exceptions.h"
#include "pushdownautomaton.h"

class PushdownAutomaton_test : public testing::Test {
public:
	using PDM = PushdownAutomaton<char, std::string>;
	PDM pdm;
};

TEST_F(PushdownAutomaton_test, isCreatedEmpty)
{
	EXPECT_EQ(0, pdm.alphabet().size()) << "Alphabet is not created empty";
	EXPECT_EQ(0, pdm.stackAlphabet().size()) << "Stack alphabet is not created empty";
	EXPECT_EQ(0, pdm.states().size()) << "State set is not created empty";
	EXPECT_EQ(0, pdm.transitions().size()) << "Transition map is not created empty";
	EXPECT_EQ(0, pdm.finalStates().size()) << "Final states set is not created empty";
}

TEST_F(PushdownAutomaton_test, insertSymbol)
{
	pdm.insertSymbol('a');
	EXPECT_EQ(1, pdm.alphabet().size()) << "Insert single symbol does not work";

	std::array<char, 2> arr = {'b', 'c'};
	pdm.insertSymbol(arr.cbegin(), arr.cend());
	EXPECT_EQ(3, pdm.alphabet().size()) << "Insert symbols with iterators does not work";

	pdm.insertSymbol({'d', 'e'});
	EXPECT_EQ(5, pdm.alphabet().size()) << "Insert symbols with initializer list does not work";

	pdm.emplaceSymbol('f');
	EXPECT_EQ(6, pdm.alphabet().size()) << "Emplace single symbol does not work";
}

TEST_F(PushdownAutomaton_test, insertDuplicatedSymbol)
{
	// initial symbol
	pdm.insertSymbol('a');

	pdm.insertSymbol('a');
	EXPECT_EQ(1, pdm.alphabet().size()) << "Insert single symbol creates duplicates";

	std::array<char, 2> ilist = {'a', 'b'};
	pdm.insertSymbol(ilist.cbegin(), ilist.cend());
	EXPECT_EQ(2, pdm.alphabet().size()) << "Insert symbols with iterators creates duplicates";

	pdm.insertSymbol({'a', 'b', 'c'});
	EXPECT_EQ(3, pdm.alphabet().size()) << "Insert symbols with initializer list creates duplicates";

	pdm.emplaceSymbol('a');
	EXPECT_EQ(3, pdm.alphabet().size()) << "Emplace single symbol creates duplicates";
}

TEST_F(PushdownAutomaton_test, removeSymbol)
{
	pdm.insertSymbol({'a', 'b'});

	EXPECT_NO_THROW(pdm.removeSymbol('a'));
	EXPECT_EQ(PDM::Alphabet{'b'}, pdm.alphabet());

	EXPECT_THROW(pdm.removeSymbol('a'), PDM::invalid_symbol);
}

TEST_F(PushdownAutomaton_test, insertStackSymbol)
{
	pdm.insertStackSymbol('a');
	EXPECT_EQ(1, pdm.stackAlphabet().size()) << "Insert single symbol does not work";

	std::array<char, 2> arr = {'b', 'c'};
	pdm.insertStackSymbol(arr.cbegin(), arr.cend());
	EXPECT_EQ(3, pdm.stackAlphabet().size()) << "Insert symbols with iterators does not work";

	pdm.insertStackSymbol({'d', 'e'});
	EXPECT_EQ(5, pdm.stackAlphabet().size()) << "Insert symbols with initializer list does not work";

	pdm.emplaceStackSymbol('f');
	EXPECT_EQ(6, pdm.stackAlphabet().size()) << "Emplace single symbol does not work";
}

TEST_F(PushdownAutomaton_test, insertDuplicatedStackSymbol)
{
	// initial symbol
	pdm.insertStackSymbol('a');

	pdm.insertStackSymbol('a');
	EXPECT_EQ(1, pdm.stackAlphabet().size()) << "Insert single symbol creates duplicates";

	std::array<char, 2> ilist = {'a', 'b'};
	pdm.insertStackSymbol(ilist.cbegin(), ilist.cend());
	EXPECT_EQ(2, pdm.stackAlphabet().size()) << "Insert symbols with iterators creates duplicates";

	pdm.insertStackSymbol({'a', 'b', 'c'});
	EXPECT_EQ(3, pdm.stackAlphabet().size()) << "Insert symbols with initializer list creates duplicates";

	pdm.emplaceStackSymbol('a');
	EXPECT_EQ(3, pdm.stackAlphabet().size()) << "Emplace single symbol creates duplicates";
}

TEST_F(PushdownAutomaton_test, removeStackSymbol)
{
	pdm.insertStackSymbol({'a', 'b'});

	EXPECT_NO_THROW(pdm.removeStackSymbol('a'));
	EXPECT_EQ(PDM::Alphabet{'b'}, pdm.stackAlphabet());

	EXPECT_THROW(pdm.removeStackSymbol('a'), PDM::invalid_symbol);
}

TEST_F(PushdownAutomaton_test, insertState)
{
	pdm.insertState("q0");
	EXPECT_EQ(1, pdm.states().size()) << "Insert single symbol does not work";

	std::array<std::string, 2> arr = {"q1", "q2"};
	pdm.insertState(arr.cbegin(), arr.cend());
	EXPECT_EQ(3, pdm.states().size()) << "Insert states with iterators does not work";

	pdm.insertState({"q3", "q4"});
	EXPECT_EQ(5, pdm.states().size()) << "Insert states with initializer list does not work";

	pdm.emplaceState("q5");
	EXPECT_EQ(6, pdm.states().size()) << "Emplace single state does not work";
}

TEST_F(PushdownAutomaton_test, insertDuplicatedState)
{
	pdm.insertState("q0");
	EXPECT_EQ(1, pdm.states().size()) << "Insert single symbol creates duplicates";

	std::array<std::string, 2> arr = {"q0", "q1"};
	pdm.insertState(arr.cbegin(), arr.cend());
	EXPECT_EQ(2, pdm.states().size()) << "Insert states with iterators creates duplicates";

	pdm.insertState({"q0", "q1", "q2"});
	EXPECT_EQ(3, pdm.states().size()) << "Insert states with initializer list creates duplicates";

	pdm.emplaceState("q0");
	EXPECT_EQ(3, pdm.states().size()) << "Emplace single state creates duplicates";
}

TEST_F(PushdownAutomaton_test, removeState)
{
	pdm.insertState({"q0", "q1"});

	EXPECT_NO_THROW(pdm.removeState("q0"));
	EXPECT_EQ(PDM::StateSet{"q1"}, pdm.states());

	// q0 is not there anymore
	EXPECT_THROW(pdm.removeState("q0"), PDM::invalid_state);
}

TEST_F(PushdownAutomaton_test, setInitialState)
{
	EXPECT_THROW(pdm.initialState("q0"), PDM::invalid_state);

	pdm.insertState("q0");
	EXPECT_NO_THROW(pdm.initialState("q0"));
	EXPECT_EQ("q0", pdm.initialState());
}

TEST_F(PushdownAutomaton_test, insertTransition)
{
	pdm.insertSymbol({'a'});
	pdm.insertStackSymbol({'$', '&'});
	pdm.insertState({"q0", "q1"});

	PDM::TransitionMap map;

	EXPECT_NO_THROW(pdm.insertTransition(std::make_tuple("q0", 'a', '$'), std::make_tuple("q1", '&')));
	map.emplace(std::make_tuple("q0", 'a', '$'), std::make_tuple("q1", '&'));
	EXPECT_EQ(map, pdm.transitions());

	EXPECT_NO_THROW(pdm.insertTransition({std::make_tuple("q1", pdm.epsilon(), '&'), std::make_tuple("q0", '$')}));
	map.emplace(std::make_tuple("q1", pdm.epsilon(), '&'), std::make_tuple("q0", '$'));
	EXPECT_EQ(map, pdm.transitions());
}

TEST_F(PushdownAutomaton_test, insertInvalidTransition)
{
	// machine is empty
	EXPECT_THROW(pdm.insertTransition(std::make_tuple("q0", 'a', '$'), std::make_tuple("q1", '&')), PDM::invalid_state);
	EXPECT_EQ(0, pdm.transitions().size());

	// "q0" exists, but 'a' is not a valid symbol
	pdm.insertState("q0");
	EXPECT_THROW(pdm.insertTransition(std::make_tuple("q0", 'a', '$'), std::make_tuple("q1", '&')),
		PDM::invalid_symbol);
	EXPECT_EQ(0, pdm.transitions().size());

	// "q0" and 'a' exists, but '$' is not a valid stack symbol
	pdm.insertSymbol('a');
	EXPECT_THROW(pdm.insertTransition(std::make_tuple("q0", 'a', '$'), std::make_tuple("q1", '&')),
		PDM::invalid_symbol);
	EXPECT_EQ(0, pdm.transitions().size());

	// "q0", 'a' and '$' exists, but "q1" is not a valid state
	pdm.insertStackSymbol('$');
	EXPECT_THROW(pdm.insertTransition(std::make_tuple("q0", 'a', '$'), std::make_tuple("q1", '&')), PDM::invalid_state);
	EXPECT_EQ(0, pdm.transitions().size());

	// "q0", 'a', '$' and "q1" exists, but '&' is not a valid stack symbol
	pdm.insertState("q1");
	EXPECT_THROW(pdm.insertTransition(std::make_tuple("q0", 'a', '$'), std::make_tuple("q1", '&')),
		PDM::invalid_symbol);
	EXPECT_EQ(0, pdm.transitions().size());
}

TEST_F(PushdownAutomaton_test, insertFinalState)
{
	pdm.insertState("q0");

	EXPECT_NO_THROW(pdm.insertFinalState("q0"));
	EXPECT_EQ(PDM::StateSet{"q0"}, pdm.finalStates());
}

TEST_F(PushdownAutomaton_test, insertInvalidFinalState)
{
	EXPECT_THROW(pdm.insertFinalState("q0"), PDM::invalid_state);
	EXPECT_EQ(0, pdm.finalStates().size());
}

TEST_F(PushdownAutomaton_test, removeFinalState)
{
	EXPECT_THROW(pdm.removeFinalState("q0"), PDM::invalid_state);

	pdm.insertState("q0");
	pdm.insertFinalState("q0");
	EXPECT_NO_THROW(pdm.removeFinalState("q0"));
	EXPECT_EQ(0, pdm.finalStates().size());
}

TEST_F(PushdownAutomaton_test, acceptAndReject)
{
	// alphabet = {a, b}
	// stack alphabet = {a, b, $}
	// language = { a^nb^n, n >= 0 }
	// image: http://i.ytimg.com/vi/QWX9CwiukL4/hqdefault.jpg
	pdm.insertSymbol({'a', 'b'});
	pdm.insertStackSymbol({'a', 'b', '$'});
	pdm.insertState({"q0", "q1", "q2", "q3"});
	pdm.initialState("q0");
	pdm.insertTransition(std::make_tuple("q0", pdm.epsilon(), pdm.epsilon()), std::make_tuple("q1", '$'));
	pdm.insertTransition(std::make_tuple("q1", 'a', pdm.epsilon()), std::make_tuple("q1", 'a'));
	pdm.insertTransition(std::make_tuple("q1", 'b', 'a'), std::make_tuple("q2", pdm.epsilon()));
	pdm.insertTransition(std::make_tuple("q2", 'b', 'a'), std::make_tuple("q2", pdm.epsilon()));
	pdm.insertTransition(std::make_tuple("q2", 'b', '$'), std::make_tuple("q3", pdm.epsilon()));
	pdm.insertFinalState("q3");

	EXPECT_TRUE(pdm.accept({'a', 'a', 'b', 'b'}));
	EXPECT_FALSE(pdm.accept({'a', 'a', 'b'}));
}
