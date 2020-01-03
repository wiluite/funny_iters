//
// Created by ET on 31.12.2019.
//

#pragma once
#include <iterator>
#include <array>
#include <algorithm>

namespace funny_it
{
    template<class, size_t N>
    class ring_buffer_sequence;

    template <class ValueType, size_t N>
    class ring_iterator: public std::iterator<std::input_iterator_tag, ValueType, ptrdiff_t, void, ValueType>
    {
    public:
        friend class ring_buffer_sequence<ValueType, N>;
        using sequence_class = ring_buffer_sequence<ValueType, N>;

        using class_type = ring_iterator<ValueType, N>;
        using value_type = ValueType;
        
    private:
        sequence_class & sequence_;
        value_type * ptr_;
        explicit ring_iterator(sequence_class & seq, ValueType const * const guard) : sequence_(seq), ptr_(guard){}

    };

    template <class V, size_t N>
    struct ring_buffer_limits
    {
    private:
        V (& buf_)[N];
    public:
        using buf_type = decltype(buf_);
        explicit ring_buffer_limits (V (& buf)[N] ) : buf_(buf) {}

        V * buffer_begin() const
        {
            return std::begin(buf_);
        }
        V * buffer_end() const
        {
            return std::end(buf_);
        }
    };

    template <class V, size_t N>
    class ring_buffer_sequence : private ring_buffer_limits<V,N>
    {
        V * head_ = buffer_begin();
        V * tail_ = buffer_begin();
        V * frame_start{};

    public:
        using class_type = ring_buffer_sequence<V,N>;
        using inherited_class_type = ring_buffer_limits<V,N>;
//        using buffer_begin = typename inherited_class_type::begin;
//        using buffer_end = typename inherited_class_type::end;
        using inherited_class_type::buffer_begin;
        using inherited_class_type::buffer_end;

        using typename inherited_class_type::buf_type ;

        using const_iterator = ring_iterator<V, N>;

        explicit ring_buffer_sequence (V (& buffer)[N]) : ring_buffer_limits<V,N>(buffer){}
        explicit ring_buffer_sequence (std::array<V, N> & arr) : inherited_class_type(reinterpret_cast<buf_type>(arr)){}

        ~ring_buffer_sequence()
        {
            static_assert(sizeof(std::array<V, N>) == sizeof(buf_type));
        }

        const_iterator begin() const
        {
            return const_iterator{*this, head_};
        }

        const_iterator end() const
        {
            return const_iterator{*this, tail_};
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
            if ((head_ + bytes_transferred) > buffer_end())
            {
                auto const rest_1 = buffer_end() - head_;
                std::copy (external_buf, external_buf + rest_1, head_);
                auto const rest_2 = head_ + bytes_transferred - buffer_end();
                std::copy (external_buf + rest_1, external_buf + rest_1 + rest_2, buffer_begin());
                head_ = buffer_begin() + rest_2;
            } else
            {
                std::copy (external_buf, external_buf + bytes_transferred, head_);
                head_ += bytes_transferred;
                if (head_ == buffer_end())
                {
                    head_ = buffer_begin();
                }
            }
        }
        bool operator == (class_type const & other) const noexcept
        {
            return (std::equal(buffer_begin(), buffer_end(), other.buffer_begin()))
                   && (head() - buffer_begin() == other.head() - other.buffer_begin())
                   && (tail() - buffer_begin() == other.tail() - other.buffer_begin());
        }

        bool operator != (class_type const & other) const noexcept
        {
            return !(*this == other);
        }

    };
}

