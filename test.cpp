//
// Created by ET on 02.01.2020.
//

#define BOOST_TEST_MODULE boost_test_module_
#include <boost/test/unit_test.hpp> // UTF ??
#include "ring_iter.h"

using namespace funny_it;

BOOST_AUTO_TEST_CASE( ring_iterator_sequence_test ) {

    char c_array[10] {};
    ring_buffer_sequence rbs1 (c_array);
    BOOST_REQUIRE (rbs1.head() == rbs1.tail());
    BOOST_REQUIRE (rbs1.head() == rbs1.buffer_begin());
    BOOST_REQUIRE (rbs1.head() == std::end(rbs1));

    char external_buffer[6] = {0x31,0x32,0x33,0x34,0x35,0x36};

    rbs1.fill_data(external_buffer, sizeof(external_buffer));
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+0), 0x31);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+1), 0x32);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+2), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+3), 0x34);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+4), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+5), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+6), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+7), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+8), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+9), 0);
    BOOST_REQUIRE (rbs1.head() - rbs1.tail() == 6);

    rbs1.fill_data(external_buffer, sizeof(external_buffer)); // rotation
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+0), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+1), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+2), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+3), 0x34);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+4), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+5), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+6), 0x31);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+7), 0x32);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+8), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.buffer_begin()+9), 0x34);
    BOOST_REQUIRE (rbs1.head() == rbs1.buffer_begin()+2 );

    std::array<char,10> std_array {};
    ring_buffer_sequence rbs2 (std_array);
    BOOST_REQUIRE(rbs1 != rbs2);
    rbs2.fill_data(external_buffer, sizeof(external_buffer));
    rbs2.fill_data(external_buffer, sizeof(external_buffer));

    BOOST_REQUIRE(rbs1 == rbs2);

    BOOST_REQUIRE (rbs1.head() != rbs1.tail());
    rbs1.align();
    BOOST_REQUIRE (rbs1.head() == rbs1.tail());
}

BOOST_AUTO_TEST_CASE( ring_iterator_standard_algorithms_test )
{
    std::array<char,10> std_array {};
    ring_buffer_sequence rbs (std_array);
    char external_buffer[6] = {0x31,0x32,0x33,0x34,0x35,0x36};
    rbs.fill_data(external_buffer, sizeof(external_buffer));

    // iteration without rotation
    for (auto  & elem : rbs)
    {
        std::cout << elem << std::endl;
    }

    // align tail with head, and fill data with rotation
    rbs.align();
    rbs.fill_data(external_buffer, sizeof(external_buffer));

    std::cout << "---" << std::endl;
    // iteration with rotation (data is still consequent incremental)
    auto current_elem = (*rbs.begin()) - 1;
    for (auto & elem : rbs)
    {
        BOOST_REQUIRE_EQUAL((int)elem, (int)current_elem + 1);
        current_elem = elem;
        std::cout << elem << std::endl;
    }

    auto const _  = std::find (rbs.begin(), rbs.end(), '6');
    static_assert (std::is_same<decltype (_), ring_buffer_iterator<char, 10> const>::value);
    static_assert (std::is_same<decltype (_), decltype(rbs)::const_iterator const>::value);
    BOOST_REQUIRE(_ != std::end(rbs));
    BOOST_REQUIRE(*_ == '6');

    std::array<char, 3> sub_seq {'4', '5', '6'};
    auto const __ = std::search (rbs.begin(), rbs.end(), sub_seq.begin(), sub_seq.end());
    BOOST_REQUIRE(__ != std::end(rbs));
    BOOST_REQUIRE(*__ == '4');
    BOOST_REQUIRE(__ == rbs.buffer_begin() + 9);
    BOOST_REQUIRE(rbs.buffer_begin() + 9 == __);
}

BOOST_AUTO_TEST_CASE( ring_iterator_align_by_iterator_test )
{
    std::array<char,10> std_array {};
    ring_buffer_sequence rbs (std_array);
    char external_buffer[10]{};
    sprintf (external_buffer, "%s", "foo#test");
    rbs.fill_data(external_buffer, strlen(external_buffer));
    auto __  = std::find (rbs.begin(), rbs.end(), '#');
    BOOST_REQUIRE (__ != std::end(rbs));
    BOOST_REQUIRE (*__ == '#');
    BOOST_REQUIRE_NO_THROW(rbs.align(__));
    BOOST_REQUIRE (rbs.tail() == rbs.buffer_begin()+ 4);

    // new search find nothing.
    BOOST_REQUIRE ((__ = std::find (rbs.begin(), rbs.end(), '#')) == std::end(rbs));
    // and you should not pass the not-succeeding iterator to
    BOOST_REQUIRE_THROW(rbs.align(__), logic_exception);
}