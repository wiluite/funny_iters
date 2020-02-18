#pragma once
#include <iterator>
#include <array>
#include <algorithm>
#include <exception>

namespace funny_it
{
    using exception_checked_variant_type = std::integral_constant<bool, true>;
    using exception_unchecked_variant_type = std::integral_constant<bool, false>;
    /*
     * Iterator belongs to the sequence that spawned it recently through begin(), end() and the sequence was not reset().
     */
    template<typename Iter>
    bool is_iter_up_to_date(Iter it) noexcept
    {
        return it.up_to_date_flag == it.sequence_->up_to_date_flag;
    }

    /**
     * \brief Outdated iterator exception
     */
    template <class It>
    class outdated_iterator : public std::exception {
        It it_;
    public:
        explicit outdated_iterator (It it) : it_(move(it)) {}
        typename It::value_type* get_iter() const
        {
            return it_.ptr_;
        }
    };

    /*
     * Throws if an iterator does not belong to the sequence that spawned it
     */
    template<typename Iter>
    void throw_if_iter_outdated(Iter const & it, exception_checked_variant_type)
    {
        if (!is_iter_up_to_date(it))
        {
            throw outdated_iterator<Iter>(it);
        }
    }

    template<typename Iter>
    void throw_if_iter_outdated(Iter const & it, exception_unchecked_variant_type) noexcept {}


    /*
     * Iterator is within the range [tail-head)
     */
    template<typename Iter>
    static bool is_iter_valid(Iter const & it) noexcept
    {
        if (it.sequence_->head_ >= it.sequence_->tail_)
        {
            if (it.ptr_ < it.sequence_->tail_)
                return false;
            return it.ptr_ <= it.sequence_->head_;
        } else
        {
            return !((it.ptr_ < it.sequence_->tail_) && (it.ptr_ > it.sequence_->head_));
        }
    }

    template <class It>
    struct out_of_bounds : public std::exception
    {
        It it_;
        explicit out_of_bounds (It it) : it_(move(it)) {}
        It get_iter() const
        {
            return it_;
        }
    };

    /*
     * Throws if an iterator is out of the range [tail-head)
     */
    template<typename Iter>
    void throw_if_iter_invalid(Iter const & it, exception_checked_variant_type)
    {
        if (!is_iter_valid(it))
        {
            throw out_of_bounds<Iter>(it);
        }
    }

    template<typename Iter>
    void throw_if_iter_invalid(Iter const & it, exception_unchecked_variant_type) noexcept {}


    template<typename Iter>
    void throw_if_iterator_abnormal(Iter const & it, exception_checked_variant_type e)
    {
        throw_if_iter_outdated(it, e);
        throw_if_iter_invalid(it, e);
    }

    template<typename Iter>
    void throw_if_iterator_abnormal(Iter const & it, exception_unchecked_variant_type) noexcept {}

    template<class, size_t, class E>
    class ring_buffer_sequence;

    template <class ValueType, size_t N, class E>
    class ring_buffer_iterator: public std::iterator<std::forward_iterator_tag, ValueType, ptrdiff_t, void, ValueType>
    {
        template<typename Iter>
        friend bool is_iter_up_to_date(Iter it) noexcept;
        template<typename Iter>
        friend bool is_iter_valid(Iter const & it) noexcept;

        template <class It>
        friend class outdated_iterator;


    public:
        friend class ring_buffer_sequence<ValueType, N, E>;
        using sequence_class = ring_buffer_sequence<ValueType, N, E>;

        using class_type = ring_buffer_iterator<ValueType, N, E>;
        using value_type = ValueType;

    private:
        sequence_class const * sequence_;
        value_type * ptr_;
        unsigned up_to_date_flag;

        explicit ring_buffer_iterator(sequence_class const * seq, ValueType * guard) : sequence_(seq), ptr_(guard), up_to_date_flag(sequence_->up_to_date_flag)
        {
        }

        class postinc_return{
            sequence_class const * sequence_;
            value_type const * ptr_;
            unsigned up_to_date_flag;

        public:
            explicit postinc_return(ring_buffer_iterator const& rbi) : sequence_(rbi.sequence_), ptr_(rbi.ptr_), up_to_date_flag(rbi.up_to_date_flag) {}
            value_type operator *() const { return *ptr_; }
        };

    public:
        ring_buffer_iterator (class_type const & other) = default;
        ring_buffer_iterator &operator =(class_type const & other)
        {
            if (this != &other)
            {
                ptr_ = other.ptr_;
                up_to_date_flag = other.up_to_date_flag;
                sequence_ = other.sequence_;
            }
            return *this;
        }
        ring_buffer_iterator (class_type && other) noexcept = default;
        ring_buffer_iterator &operator =(class_type && other) noexcept = default;

        constexpr bool operator ==(class_type const & other) const noexcept
        {
            return (ptr_ == other.ptr_) && (sequence_ == other.sequence_) && (up_to_date_flag == other.up_to_date_flag);
        }

        constexpr bool operator ==(value_type const * v) const noexcept
        {
            return ptr_ == v;
        }

        constexpr bool operator !=(class_type const & other) const noexcept
        {
            return !(*this == other);
        }

        constexpr value_type & operator *() const
        {
            throw_if_iterator_abnormal (*this, E());
            return *ptr_;
        }

        constexpr class_type & operator ++()
        {
            return *this += 1;
        }

        constexpr class_type operator +(int n) const
        {
            auto it = *this;
            it += n;
            return it;
        }

        //FIXME: overloaded operator++ returns a non const, and clang-tidy complains
        constexpr postinc_return operator ++(int)
        {
            postinc_return ret (*this);
            ++*this;
            return ret;
        }

        constexpr value_type & operator[] (typename std::iterator_traits<class_type>::difference_type d) const
        {
            return *(operator +(d));
        }

        constexpr class_type & operator +=(int n)
        {
            throw_if_iter_outdated(*this, E());
            auto tmp_ptr = sequence_->bbegin() + ((ptr_ + n - sequence_->bbegin()) % sequence_->bsize());
            throw_if_iter_invalid(class_type (sequence_, tmp_ptr), E());
            std::swap(tmp_ptr, ptr_);
            return *this;
        }

        constexpr explicit operator bool() const
        {
            return (ptr_ != sequence_->head());
        }
    };

    template <class V, size_t N, class E>
    constexpr bool operator == (V const * const value, ring_buffer_iterator<V,N,E> const & iter) noexcept
    {
        return iter == value;
    }

    template <class V, size_t N, class E>
    constexpr bool operator == (ring_buffer_iterator<V,N,E> const & iter, V const * const value) noexcept
    {
        return iter == value;
    }

    template <class V, size_t N, class E>
    constexpr ring_buffer_iterator<V,N,E> operator + (ring_buffer_iterator<V,N,E> const & iter, int n)
    {
        auto tmp(iter);
        return tmp+n;
    }

    template <class V, size_t N>
    struct ring_buffer_base
    {
    private:
        V (& buf_)[N];
    protected:
        using buf_type = decltype(buf_);
        explicit ring_buffer_base (V (& buf)[N] ) : buf_(buf) {}

        constexpr V * bbegin() const noexcept
        {
            return std::begin(buf_);
        }
        constexpr V * bend() const noexcept
        {
            return std::end(buf_);
        }
        [[nodiscard]] constexpr decltype(N) bsize() const noexcept
        {
            return N;
        }
    };

    struct iter_mixture : public std::exception {};

    template <class V, size_t N, class E = exception_checked_variant_type>
    class ring_buffer_sequence : private ring_buffer_base<V,N>
    {
        template<typename Iter>
        friend bool is_iter_up_to_date(Iter it) noexcept;
        template<typename Iter>
        friend bool is_iter_valid(Iter const & it) noexcept;

        V * head_ = bbegin();
        V * tail_ = bbegin();
        unsigned up_to_date_flag = 0;

        void update_up_to_date_flag(exception_checked_variant_type) noexcept
        {
            ++up_to_date_flag;
        }
        void update_up_to_date_flag(exception_unchecked_variant_type) noexcept {}

    public:
        using class_type = ring_buffer_sequence<V,N>;
        using inherited_class_type = ring_buffer_base<V,N>;
        using inherited_class_type::bbegin;
        using inherited_class_type::bend;
        using inherited_class_type::bsize;

        using typename inherited_class_type::buf_type ;

        using const_iterator = ring_buffer_iterator<V, N, E>;
        friend const_iterator;

        explicit constexpr ring_buffer_sequence (V (& buffer)[N]) : ring_buffer_base<V,N>(buffer){}
        explicit constexpr ring_buffer_sequence (std::array<V, N> & array) : inherited_class_type(reinterpret_cast<buf_type>(array))
        {
            static_assert (sizeof array == sizeof(buf_type));
        }

        ring_buffer_sequence (class_type const & other) = delete;
        ring_buffer_sequence &operator =(class_type const & other) = delete;

        ring_buffer_sequence (class_type && other) noexcept = delete;
        ring_buffer_sequence &operator =(class_type && other) noexcept = delete;

        void reset(const_iterator const & tail_iter, const_iterator const & head_iter) noexcept
        {
            if ((tail_iter.ptr_ != tail_) || (head_iter.ptr_ != head_))
            {
                tail_ = tail_iter.ptr_;
                head_ = head_iter.ptr_;
                update_up_to_date_flag(E());
            }
        }
        void unchecked_reset(const_iterator const & tail_iter, const_iterator const & head_iter) noexcept
        {
            if ((tail_iter.ptr_ != tail_) || (head_iter.ptr_ != head_))
            {
                tail_ = tail_iter.ptr_;
                head_ = head_iter.ptr_;
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

        struct overflow_exception
        {};
        constexpr void fill_data(V const * const external_buf, uint8_t bytes_transferred)
        {
            if (size() + bytes_transferred >= bsize())
            {
                throw overflow_exception();
            }
            if ((head_ + bytes_transferred) > bend())
            {
                auto const rest_1 = bend() - head_;
                std::copy (external_buf, external_buf + rest_1, head_);
                auto const rest_2 = head_ + bytes_transferred - bend();
                std::copy (external_buf + rest_1, external_buf + rest_1 + rest_2, bbegin());
                head_ = bbegin() + rest_2;
            } else
            {
                std::copy (external_buf, external_buf + bytes_transferred, head_);
                if ((head_ += bytes_transferred) == bend())
                {
                    head_ = bbegin();
                }
            }
        }

        constexpr bool operator ==(class_type const & other) const noexcept
        {
            return ((head_ - bbegin() == other.head_ - other.bbegin()) && (tail_ - bbegin() == other.tail_ - other.bbegin() && std::equal(bbegin(), bend(), other.bbegin())));
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
         * Sets the tail value to the position described by the passed iterator
         * @param it iterator (mainly returned by a search algorithm)
         */
        constexpr void align (const_iterator it)
        {
            tail_ = it.ptr_;
        }

        constexpr decltype(N) size() const noexcept
        {
            if (head_ >= tail_)
            {
                return head_ - tail_;
            } else
            {
                return (bend() - tail_) + (head_ - bbegin());
            }
        }

        template<typename Iter>
        constexpr typename std::iterator_traits<Iter>::difference_type distance(Iter start_it, Iter stop_it) const
        {
            static_assert(std::is_same<Iter, const_iterator>::value);
            static_assert(std::numeric_limits<typename std::iterator_traits<const_iterator>::difference_type>::is_signed);

            throw_if_iterator_abnormal(start_it, E());
            throw_if_iterator_abnormal(stop_it, E());

            if (head_ >= tail_)
            {
                return stop_it.ptr_ - start_it.ptr_;
            } else
            {
                if (stop_it.ptr_ >= start_it.ptr_)
                {
                    if (start_it.ptr_ < tail_)
                    {
                        throw iter_mixture();
                    }
                    return stop_it.ptr_ - start_it.ptr_;
                } else
                {
                    return stop_it.ptr_ + bsize() - start_it.ptr_;
                }
            }
        }
    };
}

