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

template <class V, size_t N>
static void make_ring_buffer_rotated_sequence (funny_it::ring_buffer_sequence<V, N> & rbs)
{
    char external_buffer[3] = {0x31,0x32,0x33};
    rbs.fill_data(external_buffer, sizeof(external_buffer));
    rbs.align();
    rbs.fill_data(external_buffer, sizeof(external_buffer));
}

int main(int argc, char const * argv[] )
{
    using namespace funny_it;

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

    // ring buffer demo:
    char c_array[4] {};
    ring_buffer_sequence rbs (c_array);
    make_ring_buffer_rotated_sequence(rbs); //1,2,3,1,2,3
    // and the result is "123"
    std::cout << std::string (rbs.begin(), rbs.end()) << '\n';

    return 0;
}


