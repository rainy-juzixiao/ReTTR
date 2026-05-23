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
#ifndef  RETTR_IMPLEMENTS_ITERATOR_HPP
#define RETTR_IMPLEMENTS_ITERATOR_HPP
#include <rettr/core/prerequisites.hpp>

namespace rettr::implements {
    template <typename Derived, typename ValueType>
    class bidirectional_iterator_base {
    public:
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;
        using const_reference = const ValueType&;
        using const_pointer = const ValueType*;
        using iterator_category = std::bidirectional_iterator_tag;

#if __cplusplus >= 202002L
        using iterator_concept = std::bidirectional_iterator_tag;
#endif

        Derived& derived() {        return static_cast<Derived&>(*this);
        }

        const Derived& derived() const {
            return static_cast<const Derived&>(*this);
        }

        reference operator*() noexcept(noexcept(derived().dereference())) {
            return derived().dereference();
        }

        const_reference operator*() const noexcept(noexcept(derived().dereference_const())) {
            return derived().dereference_const();
        }

        pointer operator->() noexcept(noexcept(derived().get_pointer())) {
            return derived().get_pointer();
        }

        const_pointer operator->() const noexcept(noexcept(derived().get_pointer_const())) {
            return derived().get_pointer_const();
        }

        Derived& operator++() noexcept(noexcept(derived().increment())) {
            derived().increment();
            return derived();
        }

        Derived operator++(int) noexcept(noexcept(derived().increment())) {
            Derived tmp = derived();
            derived().increment();
            return tmp;
        }

        Derived& operator--() noexcept(noexcept(derived().decrement())) {
            derived().decrement();
            return derived();
        }

        Derived operator--(int) noexcept(noexcept(derived().decrement())) {
            Derived tmp = derived();
            derived().decrement();
            return tmp;
        }

        friend bool operator==(const bidirectional_iterator_base& lhs, const bidirectional_iterator_base& rhs)
            noexcept(noexcept(lhs.derived().equal_to(rhs.derived()))) {
            return lhs.derived().equal_to(rhs.derived());
        }

        friend bool operator!=(const bidirectional_iterator_base& lhs, const bidirectional_iterator_base& rhs)
            noexcept(noexcept(lhs == rhs)) {
            return !(lhs == rhs);
        }
    };

    template <typename MapContainer, typename BaseIterator>
    class map_mapped_iterator : public bidirectional_iterator_base<
        map_mapped_iterator<MapContainer, BaseIterator>,
        typename MapContainer::mapped_type> {

        using base = bidirectional_iterator_base<
            map_mapped_iterator<MapContainer, BaseIterator>,
            typename MapContainer::mapped_type>;

        BaseIterator it_;

    public:
        using typename base::reference;
        using typename base::const_reference;
        using typename base::pointer;
        using typename base::const_pointer;

        map_mapped_iterator() = default;
        explicit map_mapped_iterator(BaseIterator it) : it_(it) {}

        template <typename OtherIter>
        map_mapped_iterator(const map_mapped_iterator<MapContainer, OtherIter>& other)
            : it_(other.it_) {}

        reference dereference() noexcept {
            return it_->second;
        }

        const_reference dereference_const() const noexcept {
            return it_->second;
        }

        pointer get_pointer() noexcept {
            return std::addressof(it_->second);
        }

        const_pointer get_pointer_const() const noexcept {
            return std::addressof(it_->second);
        }

        void increment() noexcept {
            ++it_;
        }

        void decrement() noexcept {
            --it_;
        }

        template <typename OtherIter>
        bool equal_to(const map_mapped_iterator<MapContainer, OtherIter>& other) const noexcept {
            return it_ == other.it_;
        }

        BaseIterator base() const noexcept {
            return it_;
        }
    };

    template <typename MapContainer, typename BaseIterator>
    class map_key_iterator : public bidirectional_iterator_base<
        map_key_iterator<MapContainer, BaseIterator>,
        typename MapContainer::key_type> {

        using base = bidirectional_iterator_base<
            map_key_iterator<MapContainer, BaseIterator>,
            typename MapContainer::key_type>;

        BaseIterator it_;

    public:
        using typename base::reference;
        using typename base::const_reference;
        using typename base::pointer;
        using typename base::const_pointer;

        map_key_iterator() = default;
        explicit map_key_iterator(BaseIterator it) : it_(it) {}

        reference dereference() noexcept {
            return const_cast<reference>(it_->first);
        }

        const_reference dereference_const() const noexcept {
            return it_->first;
        }

        pointer get_pointer() noexcept {
            return const_cast<pointer>(std::addressof(it_->first));
        }

        const_pointer get_pointer_const() const noexcept {
            return std::addressof(it_->first);
        }

        void increment() noexcept {
            ++it_;
        }

        void decrement() noexcept {
            --it_;
        }

        bool equal_to(const map_key_iterator& other) const noexcept {
            return it_ == other.it_;
        }

        BaseIterator base() const noexcept {
            return it_;
        }
    };

    template <typename Iterator>
    class iterator_range {
        Iterator begin_;
        Iterator end_;

    public:
        iterator_range(Iterator begin, Iterator end) : begin_(begin), end_(end) {}

        Iterator begin() const noexcept { return begin_; }
        Iterator end() const noexcept { return end_; }
    };

    template <typename Map>
    auto mapped_range(Map& map) -> iterator_range<map_mapped_iterator<Map, decltype(map.begin())>> {
        using iterator = map_mapped_iterator<Map, decltype(map.begin())>;
        return iterator_range<iterator>(iterator(map.begin()), iterator(map.end()));
    }

    template <typename Map>
    auto mapped_range(const Map& map) -> iterator_range<map_mapped_iterator<const Map, decltype(map.cbegin())>> {
        using iterator = map_mapped_iterator<const Map, decltype(map.cbegin())>;
        return iterator_range<iterator>(iterator(map.cbegin()), iterator(map.cend()));
    }

    template <typename Map>
    auto key_range(Map& map) -> iterator_range<map_key_iterator<Map, decltype(map.begin())>> {
        using iterator = map_key_iterator<Map, decltype(map.begin())>;
        return iterator_range<iterator>(iterator(map.begin()), iterator(map.end()));
    }

    template <typename Map>
    auto key_range(const Map& map) -> iterator_range<map_key_iterator<const Map, decltype(map.cbegin())>> {
        using iterator = map_key_iterator<const Map, decltype(map.cbegin())>;
        return iterator_range<iterator>(iterator(map.cbegin()), iterator(map.cend()));
    }
}

#endif