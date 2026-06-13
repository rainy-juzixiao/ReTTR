/*
 * Copyright 2026 rainy-juzixiao
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RETTR_ARRAY_RANGE_HPP
#define RETTR_ARRAY_RANGE_HPP
#include <functional>
#include <rettr/core/prerequisites.hpp>

#if RETTR_USING_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winit-list-lifetime"
#endif

namespace rettr {
    class typeinfo;

    template <typename Ty>
    struct default_predicate {
        default_predicate() {
        }

        default_predicate(std::function<bool(const Ty &)> func) : func(std::move(func)) {
        }

        bool operator()(const Ty &obj) const {
            return (func ? func(obj) : true);
        }

        std::function<bool(const Ty &)> func;
    };

    template <>
    struct default_predicate<typeinfo> {
        constexpr default_predicate() {
        }

        constexpr bool operator()(const typeinfo &) const {
            return true;
        }
    };

    template <typename Ty, typename Predicate = default_predicate<Ty>>
    class array_range {
    public:
        using value_type = Ty;
        using size_type = std::size_t;
        using reference = value_type &;
        using const_reference = value_type const &;
        using pointer = value_type *;
        using const_pointer = value_type const *;

        class iterator;
        class const_iterator;
        class reverse_iterator;
        class const_reverse_iterator;

        constexpr array_range() noexcept = default;

        constexpr array_range(pointer data, size_type size, const Predicate &pred = Predicate()) noexcept :
            data_(data), size_(size), pred_(pred) {
        }

        constexpr array_range(const_pointer data, size_type size, const Predicate &pred = Predicate()) noexcept :
            data_(const_cast<pointer>(data)), size_(size), pred_(pred) {
        }

        constexpr array_range(const_pointer first, const_pointer last, const Predicate &pred = Predicate()) noexcept :
            data_(const_cast<pointer>(first)), size_(static_cast<size_type>(last - first)), pred_(pred) {
        }

        constexpr array_range(std::initializer_list<value_type> value, const Predicate &pred = Predicate()) noexcept :
            data_(const_cast<pointer>(value.begin())), size_(value.size()), pred_(pred) {
        }

        template <typename C, size_type N>
        constexpr array_range(C (&value)[N], const Predicate &pred = Predicate()) noexcept : array_range(value, N, pred) {
        }

        template <typename C>
        RETTR_CONSTEXPR20 array_range(std::vector<C> &value, const Predicate &pred = Predicate()) noexcept :
            array_range(data(value), value.size(), pred) {
        }

        template <typename C>
        RETTR_CONSTEXPR20 array_range(const std::vector<C> &value, const Predicate &pred = Predicate()) noexcept :
            array_range(data(value), value.size(), pred) {
        }

        template <typename C, std::size_t N>
        constexpr array_range(std::array<C, N> &value, const Predicate &pred = Predicate()) noexcept :
            array_range(value.data(), value.size(), pred) {
        }

        template <typename C, std::size_t N>
        constexpr array_range(const std::array<C, N> &value, const Predicate &pred = Predicate()) noexcept :
            array_range(value.data(), value.size(), pred) {
        }

        template <typename OtherType, typename OtherPredicate,
                  std::enable_if_t<std::is_convertible_v<OtherType (*)[], Ty (*)[]>, int> = 0>
        constexpr array_range(const array_range<OtherType, OtherPredicate> &other) noexcept :
            array_range(other.data(), other.size(), Predicate()) {
        }

        constexpr reference operator[](size_type pos) noexcept {
            assert(pos < size());
            return data_[pos];
        }

        constexpr const_reference operator[](size_type pos) const noexcept {
            assert(pos < size());
            return data_[pos];
        }

        constexpr reference at(size_type pos) {
            if (size() <= pos) {
                throw std::out_of_range("array_range::at");
            }
            return data_[pos];
        }

        constexpr const_reference at(size_type pos) const {
            if (size() <= pos) {
                throw std::out_of_range("array_range::at");
            }
            return data_[pos];
        }

        constexpr reference front() noexcept {
            assert(size_ > 0);
            return *data_;
        }

        constexpr const_reference front() const noexcept {
            assert(size_ > 0);
            return *data_;
        }

        constexpr reference back() noexcept {
            assert(size_ > 0);
            return data_[size_ - 1];
        }

        constexpr const_reference back() const noexcept {
            assert(size_ > 0);
            return data_[size_ - 1];
        }

        constexpr pointer data() const noexcept {
            return data_;
        }

        iterator begin() noexcept;

        const_iterator begin() const noexcept;

        const_iterator cbegin() const noexcept;

        iterator end() noexcept;

        const_iterator end() const noexcept;

        const_iterator cend() const noexcept;

        reverse_iterator rbegin() noexcept;

        const_reverse_iterator rbegin() const noexcept;

        const_reverse_iterator crbegin() const noexcept;

        reverse_iterator rend() noexcept;

        const_reverse_iterator rend() const noexcept;

        const_reverse_iterator crend() const noexcept;

        size_type size() const noexcept;

        bool empty() const noexcept;

        const Predicate &predicate() const noexcept {
            return pred_;
        }

        void swap(array_range &right) noexcept {
            std::swap(data_, right.data_);
            std::swap(size_, right.size_);
            std::swap(pred_, right.pred_);
        }

    private:
        template <typename C>
        RETTR_CONSTEXPR20 static auto data(std::vector<C> &value) noexcept {
            static_assert(!std::is_same_v<C, bool>, "Cannot use std::vector<bool> as an array_range.");
            return value.data();
        }

        template <typename C>
        RETTR_CONSTEXPR20 static auto data(const std::vector<C> &value) noexcept {
            static_assert(!std::is_same_v<C, bool>, "Cannot use std::vector<bool> as an array_range.");
            return value.data();
        }

        pointer data_{nullptr};
        size_type size_{0};
        Predicate pred_{};
    };

    template <typename Ty, typename Predicate>
    class array_range<Ty, Predicate>::iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Ty;
        using difference_type = std::ptrdiff_t;
        using pointer = Ty *;
        using reference = Ty &;

        friend class const_iterator;

        iterator() : ptr_(nullptr), range_(nullptr) {
        }

        iterator(pointer ptr, const array_range<Ty, Predicate> *range) : ptr_(ptr), range_(range) {
            while (ptr_ != range_->data_ + range_->size_ && !range_->pred_(*ptr_)) {
                ++ptr_;
            }
        }

        reference operator*() const {
            return *ptr_;
        }

        pointer operator->() const {
            return ptr_;
        }

        iterator &operator++() {
            do {
                ++ptr_;
            } while (ptr_ != range_->data_ + range_->size_ && !range_->pred_(*ptr_));
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator &right) const {
            return ptr_ == right.ptr_;
        }

        bool operator==(const const_iterator& right) const {
            return ptr_ == right.ptr_;
        }

        bool operator!=(const iterator &right) const {
            return ptr_ != right.ptr_;
        }

        bool operator!=(const const_iterator& right) const {
            return ptr_ != right.ptr_;
        }

    private:
        pointer ptr_;
        const array_range<Ty, Predicate> *range_;
    };

    template <typename Ty, typename Predicate>
    class array_range<Ty, Predicate>::const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const Ty;
        using difference_type = std::ptrdiff_t;
        using pointer = const Ty *;
        using reference = const Ty &;

        friend class iterator;

        const_iterator() : ptr_(nullptr), range_(nullptr) {
        }

        const_iterator(pointer ptr, const array_range<Ty, Predicate> *range) : ptr_(ptr), range_(range) {
            while (ptr_ != range_->data_ + range_->size_ && !range_->pred_(*ptr_)) {
                ++ptr_;
            }
        }

        const_iterator(const iterator &right) : ptr_(right.ptr_), range_(right.range_) {
        }

        reference operator*() const {
            return *ptr_;
        }

        pointer operator->() const {
            return ptr_;
        }

        const_iterator &operator++() {
            do {
                ++ptr_;
            } while (ptr_ != range_->data_ + range_->size_ && !range_->pred_(*ptr_));
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }


        bool operator==(const iterator &right) const {
            return ptr_ == right.ptr_;
        }

        bool operator==(const const_iterator& right) const {
            return ptr_ == right.ptr_;
        }

        bool operator!=(const iterator &right) const {
            return ptr_ != right.ptr_;
        }

        bool operator!=(const const_iterator& right) const {
            return ptr_ != right.ptr_;
        }

    private:
        pointer ptr_;
        const array_range<Ty, Predicate> *range_;
    };

    template <typename Ty, typename Predicate>
    class array_range<Ty, Predicate>::reverse_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Ty;
        using difference_type = std::ptrdiff_t;
        using pointer = Ty *;
        using reference = Ty &;

        friend class const_reverse_iterator;

        reverse_iterator() : ptr_(nullptr), range_(nullptr) {
        }

        reverse_iterator(pointer ptr, const array_range<Ty, Predicate> *range) : ptr_(ptr), range_(range) {
            while (ptr_ >= range_->data_ && !range_->pred_(*ptr_)) {
                --ptr_;
            }
        }

        reference operator*() const {
            return *ptr_;
        }

        pointer operator->() const {
            return ptr_;
        }

        reverse_iterator &operator++() {
            do {
                --ptr_;
            } while (ptr_ >= range_->data_ && !range_->pred_(*ptr_));
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }


        bool operator==(const reverse_iterator &right) const {
            return ptr_ == right.ptr_;
        }

        bool operator==(const const_reverse_iterator& right) const {
            return ptr_ == right.ptr_;
        }

        bool operator!=(const reverse_iterator &right) const {
            return ptr_ != right.ptr_;
        }

        bool operator!=(const const_reverse_iterator& right) const {
            return ptr_ != right.ptr_;
        }

    private:
        pointer ptr_;
        const array_range<Ty, Predicate> *range_;
    };

    template <typename Ty, typename Predicate>
    class array_range<Ty, Predicate>::const_reverse_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const Ty;
        using difference_type = std::ptrdiff_t;
        using pointer = const Ty *;
        using reference = const Ty &;

        friend class reverse_iterator;

        const_reverse_iterator() : ptr_(nullptr), range_(nullptr) {
        }

        const_reverse_iterator(pointer ptr, const array_range<Ty, Predicate> *range) : ptr_(ptr), range_(range) {
            while (ptr_ >= range_->data_ && !range_->pred_(*ptr_)) {
                --ptr_;
            }
        }

        const_reverse_iterator(const reverse_iterator &right) : ptr_(right.ptr_), range_(right.range_) {
        }

        reference operator*() const {
            return *ptr_;
        }

        pointer operator->() const {
            return ptr_;
        }

        const_reverse_iterator &operator++() {
            do {
                --ptr_;
            } while (ptr_ >= range_->data_ && !range_->pred_(*ptr_));
            return *this;
        }

        const_reverse_iterator operator++(int) {
            const_reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const reverse_iterator &right) const {
            return ptr_ == right.ptr_;
        }

        bool operator==(const const_reverse_iterator& right) const {
            return ptr_ == right.ptr_;
        }

        bool operator!=(const reverse_iterator &right) const {
            return ptr_ != right.ptr_;
        }

        bool operator!=(const const_reverse_iterator& right) const {
            return ptr_ != right.ptr_;
        }
        
    private:
        pointer ptr_;
        const array_range<Ty, Predicate> *range_;
    };

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::iterator array_range<Ty, Predicate>::begin() noexcept {
        return iterator(data_, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_iterator array_range<Ty, Predicate>::begin() const noexcept {
        return const_iterator(data_, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_iterator array_range<Ty, Predicate>::cbegin() const noexcept {
        return const_iterator(data_, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::iterator array_range<Ty, Predicate>::end() noexcept {
        return iterator(data_ + size_, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_iterator array_range<Ty, Predicate>::end() const noexcept {
        return const_iterator(data_ + size_, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_iterator array_range<Ty, Predicate>::cend() const noexcept {
        return const_iterator(data_ + size_, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::reverse_iterator array_range<Ty, Predicate>::rbegin() noexcept {
        return reverse_iterator(data_ + size_ - 1, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_reverse_iterator array_range<Ty, Predicate>::rbegin() const noexcept {
        return const_reverse_iterator(data_ + size_ - 1, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_reverse_iterator array_range<Ty, Predicate>::crbegin() const noexcept {
        return const_reverse_iterator(data_ + size_ - 1, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::reverse_iterator array_range<Ty, Predicate>::rend() noexcept {
        return reverse_iterator(data_ - 1, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_reverse_iterator array_range<Ty, Predicate>::rend() const noexcept {
        return const_reverse_iterator(data_ - 1, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::const_reverse_iterator array_range<Ty, Predicate>::crend() const noexcept {
        return const_reverse_iterator(data_ - 1, this);
    }

    template <typename Ty, typename Predicate>
    inline typename array_range<Ty, Predicate>::size_type array_range<Ty, Predicate>::size() const noexcept {
        size_type count = 0;
        for (auto it = data_; it != data_ + size_; ++it) {
            if (pred_(*it)) {
                ++count;
            }
        }
        return count;
    }

    template <typename Ty, typename Predicate>
    inline bool array_range<Ty, Predicate>::empty() const noexcept {
        for (auto it = data_; it != data_ + size_; ++it) {
            if (pred_(*it)) {
                return false;
            }
        }
        return true;
    }

    template <typename C, std::size_t N>
    array_range(C (&value)[N]) -> array_range<C>;

    template <typename C>
    array_range(std::vector<C> &value) -> array_range<C>;

    template <typename C>
    array_range(const std::vector<C> &value) -> array_range<const C>;

    template <typename C, std::size_t N>
    array_range(std::array<C, N> &value) -> array_range<C>;

    template <typename C, std::size_t N>
    array_range(const std::array<C, N> &value) -> array_range<const C>;

    template <typename Ty>
    RETTR_CONSTEXPR20 rettr_fn make_array_range(Ty *first, Ty *last) -> array_range<Ty> {
        return array_range<Ty>(first, last);
    }

    template <typename Ty>
    RETTR_CONSTEXPR20 rettr_fn make_array_range(const Ty *first, const Ty *last) -> array_range<Ty> {
        return array_range<Ty>(first, last);
    }

    template <typename Ty, std::size_t N>
    RETTR_CONSTEXPR20 rettr_fn make_array_range(Ty (&array)[N]) -> array_range<Ty> {
        return array_range<Ty>(array);
    }

    template <typename Ty>
    RETTR_CONSTEXPR20 rettr_fn make_array_range(std::vector<Ty> &vector) -> array_range<Ty> {
        return array_range<Ty>(vector);
    }

    template <typename Ty>
    RETTR_CONSTEXPR20 rettr_fn make_array_range(const std::vector<Ty> &vector) -> array_range<const Ty> {
        return array_range<const Ty>(vector);
    }

    template <typename Ty, std::size_t N>
    RETTR_CONSTEXPR20 rettr_fn make_array_range(std::array<Ty, N> &array) -> array_range<Ty> {
        return array_range<Ty>(array);
    }

    template <typename Ty, std::size_t N>
    RETTR_CONSTEXPR20 rettr_fn make_array_range(const std::array<Ty, N> &array) -> array_range<const Ty> {
        return array_range<const Ty>(array);
    }
}

#if RETTR_USING_GCC
#pragma GCC diagnostic pop
#endif

#endif
