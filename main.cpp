#include "bit_iter.h"

#include <cassert>
#include <algorithm> // std::count
#include <numeric>   // std::accumulate

#include <iostream>

int main(int argc, char const * argv[] )
{
    BitSequence seq {make_bytes(0x0A,0x0B,0x0A)}; // 00001010  00001011  00001010 (7 bits by 1, 17 bits by 0)

    assert (std::count(std::begin(seq), std::end(seq),std::byte{1}) == 7);
    assert (std::count(std::begin(seq), std::end(seq),std::byte{0}) == 17);

    // iterate through bits
    for (auto & elem : seq)
    {
        std::cout << (int)elem << '\n';
    }

    auto b = std::accumulate(std::begin(seq), std::end(seq),int(0));
    std::cout << "b=" << b << std::endl;
    assert (b == 7);

    //std::array<char, 20> arr1 {'a','b','c','d','e','f','g','h','i', 'j'};

    //print only first byte
    for (auto b2 = seq.begin(); b2!= seq.end()-16; ++b2)
    {
        std::cout << (int)*b2 << std::endl;
    }

    return 0;
}


