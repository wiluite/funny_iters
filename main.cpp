#include "bit_iter.h"
#include "ring_iter.h"

#include <cassert>
#include <algorithm> // std::count
#include <numeric>   // std::accumulate

#include <iostream>


template<typename... Ts>
std::array<std::byte, sizeof...(Ts)> make_bytes(Ts && ... args) noexcept
{
    return {std::byte(std::forward<Ts>(args))...};
}

int main(int argc, char const * argv[] )
{
    using namespace funny_it;
    std::array<char, 10> a{};
    char a2[10];
    char a3[6] = {0x31,0x32,0x33,0x34,0x35,0x36};
    ring_buffer_sequence rbs1 (a);
    ring_buffer_sequence rbs2 (a2);
    std::cout << sizeof(a2) << std::endl;
    rbs1.fill_data(a3, sizeof(a3));
    rbs1.fill_data(a3, sizeof(a3));
    std::cout << *(rbs1.begin()+0) << std::endl;
    std::cout << *(rbs1.begin()+1) << std::endl;
    std::cout << *(rbs1.begin()+2) << std::endl;
    std::cout << *(rbs1.begin()+3) << std::endl;
    std::cout << *(rbs1.begin()+4) << std::endl;
    std::cout << *(rbs1.begin()+5) << std::endl;
    std::cout << *(rbs1.begin()+6) << std::endl;
    std::cout << *(rbs1.begin()+7) << std::endl;
    std::cout << *(rbs1.begin()+8) << std::endl;
    std::cout << *(rbs1.begin()+9) << std::endl;
//    std::cout << sizeof(a) << std::endl;
    return 0;


    bit_sequence seq {make_bytes(0x0A, 0x0B, 0x0C)}; // 00001010  00001011  00001100 (7 bits by 1, 17 bits by 0)

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

    //print only first tetrade
    for (auto _ = seq.begin(); _ != seq.begin() + 4; ++_)
    {
        std::cout << (int)*_ << std::endl;
    }

    //print only last byte
    for (auto _ = seq.end() - 8; _!= seq.end(); ++_)
    {
        std::cout << (int)*_ << std::endl;
    }

    // search for 2 consequent "1" bits: 00001010 000010|11| 00001010
    std::array<std::byte,2> ar {std::byte(1), std::byte(1)};
    auto const __ = std::search(std::begin(seq), std::end(seq), ar.begin(), ar.end());
    assert (__ - std::begin(seq) == 8);

    return 0;
}


