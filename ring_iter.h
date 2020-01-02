//
// Created by ET on 31.12.2019.
//

#pragma once
#include <iterator>
#include <array>
#include <algorithm>

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
        V (& buf_)[N];
    public:
        using buf_type = decltype(buf_);
        explicit ring_buffer_limits (V (& buf)[N] ) : buf_(buf) {}

        V * begin() const
        {
            return std::begin(buf_);
        }
        V * end() const
        {
            return std::end(buf_);
        }
    };

    template <class V, size_t N>
    class ring_buffer_sequence : private ring_buffer_limits<V,N>
    {
        V * head_ = begin();
        V * tail_ = begin();
        V * frame_start{};

    public:
        using class_type = ring_buffer_sequence<V,N>;
        using inherited_class_type = ring_buffer_limits<V,N>;
        using inherited_class_type::begin;
        using inherited_class_type::end;
        using typename inherited_class_type::buf_type ;

        explicit ring_buffer_sequence (V (& buffer)[N]) : ring_buffer_limits<V,N>(buffer){}
        explicit ring_buffer_sequence (std::array<V, N> & arr) : inherited_class_type(reinterpret_cast<buf_type>(arr)){}

        ~ring_buffer_sequence()
        {
            static_assert(sizeof(std::array<V, N>) == sizeof(buf_type));
        }

        V * head() const
        {
            return head_;
        }

        V * tail() const
        {
            return tail_;
        }

        void fill_data(V const * const external_buf, uint8_t bytes_transferred)
        {
            if ((head_ + bytes_transferred) > end())
            {
                auto const rest_1 = end() - head_;
                std::copy (external_buf, external_buf + rest_1, head_);
                auto const rest_2 = head_ + bytes_transferred - end();
                std::copy (external_buf + rest_1, external_buf + rest_1 + rest_2, begin());
            } else
            {
                std::copy (external_buf, external_buf + bytes_transferred, head_);
                head_ += bytes_transferred;
                if (head_ == end())
                {
                    head_ = begin();
                }
            }
        }
        bool operator == (class_type const & other) const noexcept
        {
            return (std::equal(begin(), end(), other.begin())) && (head() == other.head()) && (tail() == other.tail());
        }

        bool operator != (class_type const & other) const noexcept
        {
            return !(*this == other);
        }
    };
}

