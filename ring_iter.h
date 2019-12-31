//
// Created by ET on 31.12.2019.
//

#pragma once
#include <iterator>

template <class ValueType>
class RingIterator: public std::iterator<std::input_iterator_tag, ValueType, ptrdiff_t, void, ValueType>
{

};

