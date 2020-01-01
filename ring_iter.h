//
// Created by ET on 31.12.2019.
//

#pragma once
#include <iterator>
#include <array>

namespace funny_it
{
    template <class ValueType>
    class ring_iterator: public std::iterator<std::input_iterator_tag, ValueType, ptrdiff_t, void, ValueType>
    {

    };

    template <class V, size_t N>
    struct ring_buffer_limits
    {
    private:
        V (& arr_)[N];
    public:
        using arr_type = decltype(arr_);
        explicit ring_buffer_limits (V (& arr)[N] ) : arr_(arr)
        {
           // std::cout << N << std::endl;
        }
        [[nodiscard]] const V * begin() const
        {
            return std::begin(arr_);
        }
        [[nodiscard]] const V * end() const
        {
            return std::end(arr_);
        }
    };

    template <class V, size_t N>
    class ring_buffer_sequence : private ring_buffer_limits<V,N>
    {
    public:
        using inherited_class_type = ring_buffer_limits<V,N>;
        using inherited_class_type::begin;
        using inherited_class_type::end;
        using raw_arr_type = typename inherited_class_type::arr_type ;

        explicit ring_buffer_sequence (V (& arr)[N] ) : ring_buffer_limits<V,N>(arr) {}
        explicit ring_buffer_sequence (std::array<V, N> & arr ) : inherited_class_type (reinterpret_cast<raw_arr_type>(arr))
        {
//            std::cout << "this constructor \n";
//            std::cout << (int)*begin() << std::endl;
//            std::cout << (int)*(end()-1) << std::endl;
        }
        ~ring_buffer_sequence()
        {
            static_assert(sizeof(std::array<V, N>) == sizeof(raw_arr_type));
        }
    };


//    std::array<char, 20> arr1 {'a','f','g','h','b','c','d','e','f','f','g','h', 'j', '1', '1', '1', '1', '1', '1', '1'};
//    std::array<char, 3> arr2 {'f','g','h'};
//    auto const _ = std::find_end (arr1.rbegin(), arr1.rend(), arr2.rbegin(), arr2.rend());
//    std::cout << "order " << _-std::rbegin(arr1);

//    int counter = 0;
//    auto _ = std::find_if (arr1.begin(), arr1.end(), [&counter](char c)
//    {
//        if ((c == 'f') || (c == 'g') || (c == 'h'))
//        {
//            if (c == 'f')
//            {
//                counter = 1;
//                return false;
//            }
//            if ((c == 'g') && (counter == 1))
//            {
//                ++counter;
//                return false;
//            }
//            if ((c == 'h') && (counter == 2))
//            {
//                return true;
//            } else
//            {
//                counter = 0;
//                return false;
//            }
//        } else
//        {
//            return false;
//        }
//
//    }
//    );
//    std::cout << "offset " << _- arr1.begin() << std::endl;

}

