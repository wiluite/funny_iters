//
// Created by ET on 02.01.2020.
//

#define BOOST_TEST_MODULE boost_test_module_
#include <boost/test/unit_test.hpp> // UTF ??
#include "ring_iter.h"

using namespace funny_it;

BOOST_AUTO_TEST_CASE( fill_data ) {

    char c_array[10] {};
    ring_buffer_sequence rbs1 (c_array);
    char external_buffer[6] = {0x31,0x32,0x33,0x34,0x35,0x36};

    rbs1.fill_data(external_buffer, sizeof(external_buffer));
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+0), 0x31);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+1), 0x32);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+2), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+3), 0x34);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+4), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+5), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+6), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+7), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+8), 0);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+9), 0);

    rbs1.fill_data(external_buffer, sizeof(external_buffer)); // rotation
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+0), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+1), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+2), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+3), 0x34);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+4), 0x35);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+5), 0x36);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+6), 0x31);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+7), 0x32);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+8), 0x33);
    BOOST_REQUIRE_EQUAL(*(rbs1.begin()+9), 0x34);
}

