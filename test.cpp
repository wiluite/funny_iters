//
// Created by ET on 02.01.2020.
//

#define BOOST_TEST_MODULE boost_test_module_
#include <boost/test/unit_test.hpp> // UTF ??
#include "ring_iter.h"
#include <iostream>

using namespace funny_it;

template <class V, size_t N>
static void make_rotated_sequence (ring_buffer_sequence<V, N> & rbs)
{
    char external_buffer[6] = {0x31,0x32,0x33,0x34,0x35,0x36};
    rbs.fill_data(external_buffer, sizeof(external_buffer));
    rbs.align();
    rbs.fill_data(external_buffer, sizeof(external_buffer));
    BOOST_REQUIRE (rbs.tail() - rbs.head() == 4);
}

BOOST_AUTO_TEST_CASE (ring_buffer_iterator_inc_operators)
{
    char c_array[10] {};
    ring_buffer_sequence rbs1 (c_array);

    make_rotated_sequence (rbs1);

    {
        auto iter = rbs1.begin();
        // inc with no rotation yet
        BOOST_REQUIRE (*iter == 0x31);
        BOOST_REQUIRE_NO_THROW (iter = iter+2);
        //BOOST_REQUIRE_NO_THROW(iter += 2);
        BOOST_REQUIRE (*iter == 0x33);
    }

    {
        auto iter = rbs1.begin();
        // inc with rotation
        BOOST_REQUIRE_NO_THROW(iter += 5);
        BOOST_REQUIRE (*iter == 0x36);
    }

    {
        using iter_type = typename decltype(rbs1)::const_iterator;
        auto iter1 = rbs1.begin();
        auto const iter2 = iter1;
        // inc with exception
        BOOST_REQUIRE_THROW(iter1 += 7, out_of_bounds<iter_type>);
        BOOST_REQUIRE(iter1 == iter2);
    }
    {
        // operator + and operator += and operator ++
        auto iter1 = rbs1.begin();
        auto iter2 = rbs1.begin();
        auto iter3 = rbs1.begin();
        iter1 = iter1 + 3;
        iter2 += 3;
        ++++++iter3;
        BOOST_REQUIRE(iter1 == iter2);
        BOOST_REQUIRE(iter1 == iter3);
    }
}

BOOST_AUTO_TEST_CASE( ring_iterator_sequence_comparison ) {

    char c_array[10] {};
    ring_buffer_sequence rbs1 (c_array);
    BOOST_REQUIRE (rbs1.head() == rbs1.tail());
    BOOST_REQUIRE (rbs1.head() == rbs1.bbegin());
    BOOST_REQUIRE (rbs1.head() == std::end(rbs1));
    BOOST_REQUIRE (rbs1.bsize() == 10);

    char external_buffer[6] = {0x31,0x32,0x33,0x34,0x35,0x36};

    rbs1.fill_data(external_buffer, sizeof(external_buffer));
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 0), 0x31);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 1), 0x32);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 2), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 3), 0x34);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 4), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 5), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 6), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 7), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 8), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 9), 0);
    BOOST_REQUIRE (rbs1.head() - rbs1.tail() == 6);

    rbs1.fill_data(external_buffer, sizeof(external_buffer)); // rotation
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 0), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 1), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 2), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 3), 0x34);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 4), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 5), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 6), 0x31);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 7), 0x32);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 8), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.bbegin() + 9), 0x34);
    BOOST_REQUIRE (rbs1.head() == rbs1.bbegin() + 2 );

    std::array<char,10> std_array {};
    ring_buffer_sequence rbs2 (std_array);
    BOOST_REQUIRE(rbs1 != rbs2);
    rbs2.fill_data(external_buffer, sizeof(external_buffer));
    rbs2.fill_data(external_buffer, sizeof(external_buffer));

    BOOST_REQUIRE(rbs1 == rbs2);
}

BOOST_AUTO_TEST_CASE( ring_iterator_sequence_align_and_size ) {
    std::array<char,10> std_array {};
    ring_buffer_sequence rbs3 (std_array);
    char external_buffer[6] = {0x31,0x32,0x33,0x34,0x35,0x36};

    rbs3.fill_data(external_buffer, sizeof(external_buffer));
    BOOST_REQUIRE_EQUAL (rbs3.size(), 6);
    BOOST_REQUIRE (rbs3.head() > rbs3.tail());
    rbs3.align();
    rbs3.fill_data(external_buffer, sizeof(external_buffer));
    BOOST_REQUIRE (rbs3.head() < rbs3.tail());
    BOOST_REQUIRE_EQUAL (rbs3.size(), 6);
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
    auto __ = std::search (rbs.begin(), rbs.end(), sub_seq.begin(), sub_seq.end());
    BOOST_REQUIRE(__ != std::end(rbs));
    BOOST_REQUIRE(*__ == '4');
    BOOST_REQUIRE(__ == rbs.bbegin() + 9);
    BOOST_REQUIRE(rbs.bbegin() + 9 == __);

    ++__;
    auto ___ = __;
    static_assert(std::is_same<decltype(__), typename std::decay<decltype(___)>::type>::value);
    std::array<char, 2> sub_seq2 {'5', '6'};
    BOOST_REQUIRE ((___ = std::search (___, rbs.end(), sub_seq2.begin(), sub_seq2.end())) != std::end(rbs));
    std::cout << *___ << std::endl;
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
    BOOST_REQUIRE_NO_THROW(rbs.align(__+1));
    BOOST_REQUIRE (rbs.tail() == rbs.bbegin()+ 4);

    // new search find nothing.
    BOOST_REQUIRE ((__ = std::find (rbs.begin(), rbs.end(), '#')) == std::end(rbs));
    using iter_type = typename decltype(rbs)::const_iterator;
    BOOST_REQUIRE_THROW(rbs.align(__+1), out_of_bounds<iter_type>);
}

BOOST_AUTO_TEST_CASE( ring_iterator_distance_test)
{
    {
        std::array<char,10> std_array {};
        ring_buffer_sequence rbs (std_array);
        char external_buffer[5]{};
        rbs.fill_data(external_buffer, sizeof(external_buffer));

        BOOST_REQUIRE_NO_THROW(rbs.distance(++rbs.begin(), ++++rbs.begin()));
        BOOST_REQUIRE_EQUAL(rbs.distance(++rbs.begin(),++++rbs.begin()), 1);

        typename decltype(rbs)::const_iterator cit1 = rbs.begin();
        typename decltype(rbs)::const_iterator cit2 = rbs.begin();

        rbs.align(rbs.begin()+1); //rbs.tail() == rbs.buffer_begin()+ 1

        using iter_type = typename decltype(rbs)::const_iterator;

        BOOST_REQUIRE_THROW(rbs.distance(cit1,cit2), out_of_bounds<iter_type>); // cit1, cit2 out of range
        ++cit1;
        BOOST_REQUIRE_THROW(rbs.distance(cit1,cit2), out_of_bounds<iter_type>); // cit2 out of range
        ++cit2;
        BOOST_REQUIRE_NO_THROW(rbs.distance(cit1,cit2)); // both are in range
        ++++++++cit1, ++++++++cit2;

        BOOST_REQUIRE_NO_THROW(rbs.distance(cit1,cit2)); // both are still in range (at head position)
        auto dist = rbs.distance(cit1,cit2);
        BOOST_REQUIRE (dist == 0);

        BOOST_REQUIRE (cit1 == rbs.head());
        BOOST_REQUIRE (cit2 == rbs.head());
        BOOST_REQUIRE_THROW(++cit2, out_of_bounds<iter_type>);
        BOOST_REQUIRE (cit2 == cit1);
    }
    {
        char c_array[10] {};
        ring_buffer_sequence rbs (c_array);
        BOOST_REQUIRE_EQUAL(rbs.distance(rbs.begin(), rbs.end()), 0);
        make_rotated_sequence (rbs);
        BOOST_REQUIRE_EQUAL(rbs.distance(rbs.begin(), rbs.end()), 6);
        BOOST_REQUIRE_THROW(rbs.distance(rbs.end(), rbs.begin()), iter_mixture);
        BOOST_REQUIRE_THROW(rbs.distance(rbs.end(), ++rbs.begin()), iter_mixture);
        BOOST_REQUIRE_EQUAL(rbs.distance(++rbs.begin(), rbs.end()), 5);
    }
}

using type1 = funny_it::ring_buffer_iterator<char, 10>;
bool exception_insufficient_data (outdated_iterator<type1> const & e)
{
    //printf("%s\n", e.get_iter()); // fixme
    return true;
}

BOOST_AUTO_TEST_CASE( ring_iterator_reset_test )
{
    std::array<char,10> std_array {};
    ring_buffer_sequence rbs (std_array);
    char external_buffer[5]{};
    rbs.fill_data(external_buffer, sizeof(external_buffer));

    auto rbs_begin = rbs.begin();
    auto rbs_end = rbs.begin();
    ++rbs_begin;
    ++++rbs_end;
    auto new_rbs_begin = rbs_begin;

    rbs.reset(rbs.begin(), rbs.end());
    BOOST_REQUIRE_NO_THROW(rbs.distance(rbs_begin, rbs_end));
    rbs.reset(new_rbs_begin, rbs.end());
    BOOST_REQUIRE_EXCEPTION(rbs.distance(rbs_begin, rbs_end), outdated_iterator<type1>, exception_insufficient_data);
}
