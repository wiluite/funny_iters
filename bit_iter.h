#pragma once

#include <array>
#include <cstddef>   // std::byte

namespace funny_it
{
    template<size_t Bytes>
    class bit_sequence;

    template<typename ValueType, size_t Bytes>
    class bit_iterator : public std::iterator<std::forward_iterator_tag, ValueType, ptrdiff_t, void, ValueType> {
    public:
        friend class bit_sequence<Bytes>;

        using class_type = bit_iterator<ValueType, Bytes>;
        using value_type = ValueType;

        mutable typename std::remove_const<ValueType>::type value;

    private:
        static_assert(std::is_same<value_type *, std::byte const *>::value);
        value_type *current_byte = nullptr;
        int8_t current_bit = 0;
        static_assert(std::numeric_limits<decltype(current_bit)>::is_signed);

    private:
        explicit bit_iterator(std::byte const *ptr) : current_byte(ptr), current_bit(0) {}

    public:
        bit_iterator(bit_iterator const & other) = default;
        bit_iterator &operator=( bit_iterator const & other) = default;
        bit_iterator(bit_iterator && other) noexcept = default;
        bit_iterator &operator=(bit_iterator && other) noexcept = default;

        bool operator == (class_type const & other) const noexcept
        {
            return (current_byte == other.current_byte) && (current_bit == other.current_bit);
        }

        bool operator != (class_type const & other) const noexcept
        {
            return !(*this == other);
        }

        value_type & operator * () const noexcept
        {
            return value = (((*current_byte) & (std::byte(1) << current_bit)) > std::byte(0)) ? std::byte{1} : std::byte{0};
        }

        class_type & operator ++ () noexcept
        {
            if (++current_bit == 8)
            {
                current_bit = 0;
                ++current_byte;
            }
            return *this;
        }

        class_type & operator += (int n) noexcept
        {
            while (n--)
            {
                operator++();
            }
            return *this;
        }

        class_type & operator -- () noexcept
        {
            if (--current_bit < 0)
            {
                current_bit = 7;
                --current_byte;
            }
            return *this;
        }

        template <class T>
        int operator - (T && other) const noexcept
        {
            return (current_byte - std::forward<T>(other).current_byte) * 8 + (current_bit - std::forward<T>(other).current_bit);
        }

        class_type operator - (int n) const noexcept
        {
            class_type tmp(*this);
            tmp -= n;
            return tmp;
        }

        class_type & operator -= (int n) noexcept
        {
            while (n--)
            {
                operator--();
            }
            return *this;
        }

        class_type operator ++ (int)
        {
            class_type ret(*this);
            operator++();
            return ret;
        }
    };

    template<typename ValueType, size_t Bytes>
    bit_iterator<ValueType, Bytes> operator+(bit_iterator<ValueType, Bytes> const &obj, int n)
    {
        bit_iterator<ValueType, Bytes> tmp(obj);
        return tmp += n;
    }


    template<size_t Bytes>
    class bit_sequence
    {
        std::array<std::byte, Bytes> arr_;

    public:
        explicit bit_sequence(std::array<std::byte, Bytes> arr) : arr_(std::move(arr)){}

        using const_iterator = bit_iterator<std::byte const, Bytes>;

        [[nodiscard]] const_iterator begin() const
        {
            return const_iterator{std::begin(arr_)};
        }

        [[nodiscard]]const_iterator end() const
        {
            return const_iterator{std::end(arr_)};
        }

        [[nodiscard]]size_t size() const
        {
            return sizeof(std::byte) * 8 * (std::end(arr_) - std::begin(arr_));
        }
    };
}

namespace std // for accumulate check
{
    int operator+(int const & v1, funny_it::bit_iterator<std::byte const, 3>::value_type v2)
    {
        return v1 + std::to_integer<int>(v2);
    }
}

