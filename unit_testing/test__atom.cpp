#include <string>
#include <typeinfo>
#include <iostream>

#include "test.hpp"
#include "hash_of.hpp"
#include "cppa/util.hpp"

#include "cppa/cppa.hpp"

using std::cout;
using std::endl;
using std::string;

using namespace cppa;
using namespace cppa::util;

static constexpr auto s_foo = atom("FooBar");

template<atom_value AtomValue, typename... Types>
void foo()
{
    cout << "foo(" << static_cast<std::uint64_t>(AtomValue)
                   << " = "
                   << to_string(AtomValue)
         << ")"
         << endl;
}

size_t test__atom()
{
    bool matched_pattern[3] = { false, false, false };
    CPPA_TEST(test__atom);
    CPPA_CHECK_EQUAL(to_string(s_foo), "FooBar");
    self() << make_tuple(atom("foo"), static_cast<std::uint32_t>(42))
           << make_tuple(atom(":Attach"), atom(":Baz"), "cstring")
           << make_tuple(atom("b"), atom("a"), atom("c"), 23.f)
           << make_tuple(atom("a"), atom("b"), atom("c"), 23.f);
    auto pattern =
    (
        on<atom("foo"), std::uint32_t>() >> [&](std::uint32_t value)
        {
            matched_pattern[0] = true;
            CPPA_CHECK_EQUAL(value, 42);
        },
        on<atom(":Attach"), atom(":Baz"), string>() >> [&](const string& str)
        {
            matched_pattern[1] = true;
            CPPA_CHECK_EQUAL(str, "cstring");
        },
        on<atom("a"), atom("b"), atom("c"), float>() >> [&](float value)
        {
            matched_pattern[2] = true;
            CPPA_CHECK_EQUAL(value, 23.f);
        }
    );
    for (auto i = 0; i < 3; ++i) receive(pattern);
    CPPA_CHECK(matched_pattern[0] && matched_pattern[1] && matched_pattern[2]);
    message msg = receive();
    CPPA_CHECK((match<atom("b"), atom("a"), atom("c"), float>(msg.content())));
    CPPA_CHECK(try_receive(msg) == false);
    return CPPA_TEST_RESULT;
}