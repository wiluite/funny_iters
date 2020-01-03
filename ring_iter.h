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
    class ring_buffer_iterator: public std::iterator<std::input_iterator_tag, ValueType, ptrdiff_t, void, ValueType>
    {
    public:
        friend class ring_buffer_sequence<ValueType, N>;
        using sequence_class = ring_buffer_sequence<ValueType, N>;

        using class_type = ring_buffer_iterator<ValueType, N>;
        using value_type = ValueType;

    private:
        sequence_class const * sequence_;
        value_type * ptr_;
        explicit ring_buffer_iterator(sequence_class const * seq, ValueType * guard) : sequence_(seq), ptr_(guard){}

    public:
        ring_buffer_iterator(class_type const & other) = default;
        ring_buffer_iterator &operator=(class_type const & other) = default;
        ring_buffer_iterator(class_type && other) noexcept = default;
        ring_buffer_iterator &operator=(class_type && other) noexcept = default;

        bool operator == (class_type const & other) const noexcept
        {
            return (ptr_ == other.ptr_);
        }

        bool operator == (value_type const * v) const noexcept
        {
            return ptr_ == v;
        }

        bool operator != (class_type const & other) const noexcept
        {
            return !(*this == other);
        }

        value_type & operator * () const noexcept
        {
            return *ptr_;
        }

        class_type & operator ++ () noexcept
        {
            if (++ptr_ == sequence_->buffer_end())
            {
                ptr_ = sequence_->buffer_begin();
            }
            return *this;
        }

    };

    template <class V, size_t N>
    bool operator == (V const * const value, ring_buffer_iterator<V,N> const & iter)
    {
        return iter == value;
    }

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
        using inherited_class_type::buffer_begin;
        using inherited_class_type::buffer_end;

        using typename inherited_class_type::buf_type ;

        using const_iterator = ring_buffer_iterator<V, N>;

        explicit ring_buffer_sequence (V (& buffer)[N]) : ring_buffer_limits<V,N>(buffer){}
        explicit ring_buffer_sequence (std::array<V, N> & array) : inherited_class_type(reinterpret_cast<buf_type>(array))
        {
            static_assert (sizeof array == sizeof(buf_type));
        }

        const_iterator begin() const
        {
            return const_iterator{this, tail_};
        }

        const_iterator end() const
        {
            return const_iterator{this, head_};
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

        void align()
        {
            tail_ = head_;
        }

    };
}

