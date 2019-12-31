#pragma once

#include <array>
#include <cstddef>   // std::byte

template <size_t Bytes>
class BitSequence;

template<typename ValueType, size_t Bytes>
class BitIterator: public std::iterator<std::input_iterator_tag, ValueType, ptrdiff_t, void, ValueType>
{
public:
    friend class BitSequence<Bytes>;

    using class_type = BitIterator<ValueType, Bytes>;
    using value_type = ValueType;

    mutable typename std::remove_const<ValueType>::type value;

private:
    static_assert(std::is_same<value_type * , std::byte const * >::value);
    value_type * current_byte;
    int8_t current_bit;
    static_assert(std::numeric_limits<decltype(current_bit)>::is_signed);

private:
    explicit BitIterator(std::byte const * ptr) : current_byte (ptr), current_bit(0) {}

public:
    // ?
    BitIterator(const BitIterator & other) = default;
    BitIterator& operator=(const BitIterator & other) = delete;

    BitIterator(BitIterator && other) noexcept = default;
    BitIterator& operator=(BitIterator && other) noexcept = default; //delete;
    ~BitIterator() noexcept = default;

    bool operator ==(BitIterator const& other) const noexcept
    {
        //return !(*this != other);
        return (current_byte == other.current_byte) && (current_bit == other.current_bit);
    }

    bool operator !=(BitIterator const& other) const noexcept
    {
        //return current_byte != other.current_byte;
        return !(*this == other);
    }

    value_type & operator *() const noexcept
    {
        value = (((*current_byte) & (std::byte(1) << current_bit)) > std::byte(0)) ? std::byte{1} : std::byte{0};
        return value;
    }

    class_type & operator ++() noexcept
    {
        if (++current_bit == 8)
        {
            current_bit = 0;
            ++current_byte;
        }
        return *this;
    }

    class_type & operator -= (int n)
    {
        //int c_byte = std::to_integer<int>(current_byte);
        while (n--)
        {
            --current_bit;
            if (current_bit < 0)
            {
                current_bit = 7;
                //--c_byte;
                --current_byte;
            }
        }
        //current_byte = c_byte;
        return *this;
    }

    class_type operator ++(int)
    {
        class_type ret(*this);
        operator ++();
        return ret;
    }
};

template<typename ValueType, size_t Bytes>
BitIterator<ValueType, Bytes> operator - (BitIterator<ValueType, Bytes> const & obj, int n)
{
    BitIterator<ValueType, Bytes> tmp (obj);
    return tmp -= n;
}

namespace std // for accumulate check
{
    int operator+ (int const & v1, BitIterator<const std::byte, 3>::value_type v2)
    {
        return v1 + std::to_integer<int>(v2);
    }
}

template <size_t Bytes>
class BitSequence {

    std::array<std::byte,Bytes> arr_;

public:
    explicit BitSequence(std::array<std::byte, Bytes> arr) : arr_(std::move(arr))
    {
    }

    using const_iterator = BitIterator<const std::byte, Bytes>;

    [[nodiscard]] const_iterator begin() const
    {
        return const_iterator{std::begin(arr_)};
    }

    [[nodiscard]]const_iterator end() const
    {
        return const_iterator{std::end(arr_)};
    }

    [[nodiscard]]size_t size () const
    {
        return sizeof(std::byte) * 8 * (std::end(arr_) - std::begin(arr_));
    }

};

template<typename... Ts>
std::array<std::byte, sizeof...(Ts)> make_bytes(Ts&&... args) noexcept
{
    return {std::byte(std::forward<Ts>(args))...};
}
