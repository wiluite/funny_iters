//
// Created by ET on 31.12.2019.
//

#pragma once
#include <iterator>
#include <array>
#include <algorithm>
#include <exception>

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
        unsigned stage_;

        explicit ring_buffer_iterator(sequence_class const * seq, ValueType * guard) : sequence_(seq), ptr_(guard), stage_(sequence_->stage_)
        {
        }

    public:
        ring_buffer_iterator (class_type const & other) = default;
        ring_buffer_iterator &operator =(class_type const & other)
        {
            if (this != &other)
            {
                ptr_ = other.ptr_;
                stage_ = other.stage_;
            }
            return *this;
        }
        ring_buffer_iterator (class_type && other) noexcept = default;
        ring_buffer_iterator &operator =(class_type && other) noexcept = default;

        constexpr bool operator ==(class_type const & other) const noexcept
        {
            return (ptr_ == other.ptr_);
        }

        constexpr bool operator ==(value_type const * v) const noexcept
        {
            return ptr_ == v;
        }

        constexpr bool operator !=(class_type const & other) const noexcept
        {
            return !(*this == other);
        }

        constexpr value_type & operator *() const noexcept
        {
            return *ptr_;
        }

        constexpr class_type & operator ++() noexcept
        {
            if (++ptr_ == sequence_->buffer_end())
            {
                ptr_ = sequence_->buffer_begin();
            }
            return *this;
        }
    };

    template <class V, size_t N>
    constexpr bool operator == (V const * const value, ring_buffer_iterator<V,N> const & iter) noexcept
    {
        return iter == value;
    }

    template <class V, size_t N>
    struct ring_buffer_limits
    {
    private:
        V (& buf_)[N];
    protected:
        using buf_type = decltype(buf_);
        explicit ring_buffer_limits (V (& buf)[N] ) : buf_(buf) {}

        constexpr V * buffer_begin() const noexcept
        {
            return std::begin(buf_);
        }
        constexpr V * buffer_end() const noexcept
        {
            return std::end(buf_);
        }
        [[nodiscard]] constexpr decltype(N) buffer_size() const noexcept
        {
            return N;
        }
    };

    struct logic_exception : public std::exception {};
    struct out_of_bounds : public std::exception {};
    struct iter_mixture : public std::exception {};
    struct outdated_iterator : public std::exception {};

    template <class V, size_t N>
    class ring_buffer_sequence : private ring_buffer_limits<V,N>
    {
        V * head_ = buffer_begin();
        V * tail_ = buffer_begin();
        unsigned stage_ = 0;

        template<typename Iter>
        bool is_iter_valid(Iter it) const noexcept
        {
            static_assert(std::is_same<Iter, const_iterator>::value);
            if (head_ >= tail_)
            {
                if (&(*it) < tail_)
                    return false;
                return &(*it) < head_;
            } else
            {
                return !((&(*it) < tail_) && (&(*it) >= head_));
            }
        }

        template<typename Iter>
        bool is_iter_up_to_date(Iter it) const noexcept
        {
            return it.stage_ == (*this).stage_;
        }

    public:
        using class_type = ring_buffer_sequence<V,N>;
        using inherited_class_type = ring_buffer_limits<V,N>;
        using inherited_class_type::buffer_begin;
        using inherited_class_type::buffer_end;
        using inherited_class_type::buffer_size;

        using typename inherited_class_type::buf_type ;

        using const_iterator = ring_buffer_iterator<V, N>;
        friend const_iterator;

        explicit constexpr ring_buffer_sequence (V (& buffer)[N]) : ring_buffer_limits<V,N>(buffer){}
        explicit constexpr ring_buffer_sequence (std::array<V, N> & array) : inherited_class_type(reinterpret_cast<buf_type>(array))
        {
            static_assert (sizeof array == sizeof(buf_type));
        }

        void reset(const_iterator tail_iter, const_iterator head_iter)
        {
            assert (tail_iter == *&tail_iter);
            if ((&*tail_iter != tail_) || (&*head_iter != head_))
            {
                tail_ = &*tail_iter;
                head_ = &*head_iter;
                ++stage_;
            }
        }

        constexpr const_iterator begin() const noexcept
        {
            return const_iterator {this, tail_};
        }

        constexpr const_iterator end() const noexcept
        {
            return const_iterator {this, head_};
        }

        constexpr V * head() const noexcept
        {
            return head_;
        }

        constexpr V * tail() const noexcept
        {
            return tail_;
        }

        constexpr void fill_data(V const * const external_buf, uint8_t bytes_transferred)
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

        constexpr bool operator ==(class_type const & other) const noexcept
        {
            return (std::equal(buffer_begin(), buffer_end(), other.buffer_begin()))
                   && (head() - buffer_begin() == other.head() - other.buffer_begin())
                   && (tail() - buffer_begin() == other.tail() - other.buffer_begin());
        }

        constexpr bool operator !=(class_type const & other) const noexcept
        {
            return !(*this == other);
        }

        constexpr void align() noexcept
        {
            tail_ = head_;
        }

        /**
         * Sets the tail value to the position following the position described by the passed iterator
         * @param it iterator (mainly returned by a search algorithm)
         */
        constexpr void align (const_iterator it)
        {
            if (&(*it) == head_)
            {
                throw logic_exception();
            }
            tail_ = &(*(++it));
        }

        constexpr void strict_align (const_iterator it)
        {
            if (&(*it) == head_)
            {
                throw logic_exception();
            }
            tail_ = &(*(it));
        }

        // several helpers for future

        constexpr decltype(N) size() const noexcept
        {
            if (head_ >= tail_)
            {
                return head_ - tail_;
            } else
            {
                return (buffer_end() - tail_) + (head_ - buffer_begin());
            }
        }

        template<typename Iter>
        typename std::iterator_traits<Iter>::difference_type distance(Iter it1, Iter it2) const
        {
            static_assert(std::is_same<Iter, const_iterator>::value);
            static_assert(std::numeric_limits<typename std::iterator_traits<const_iterator>::difference_type>::is_signed);
            if ((!is_iter_up_to_date(it1)) || (!is_iter_up_to_date(it2)))
            {
                throw outdated_iterator();
            }

            if ((!is_iter_valid(it1)) || (!is_iter_valid(it2)))
                throw out_of_bounds();

            typename std::iterator_traits<Iter>::difference_type diff = 0;
            while (it1 != it2)
            {
                if (&(*(++it1)) == head_)
                {
                    throw iter_mixture();
                }
                ++diff;
            }
            return diff;
        }
    };
}

