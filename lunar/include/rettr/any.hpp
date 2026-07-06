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
#ifndef RETTR_ANY_HPP
#define RETTR_ANY_HPP


// NOLINTBEGIN

#include <ostream>

#include <rettr/core/function_traits.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/any/cast.hpp>
#include <rettr/implements/any/execution_policy.hpp>
#include <rettr/implements/any/fwd.hpp>
#include <rettr/implements/any/iteator.hpp>
#include <rettr/implements/any/matcher.hpp>
#include <rettr/implements/any/raii_manager.hpp>

// NOLINTEND

#if RETTR_HAS_CXX20
#include <sstream>
#endif

#if RETTR_USING_MSVC
#pragma warning(push)
#pragma warning(disable : 4715 4702 6011 26439 26495)
#endif

#define RETTR_ANY_CAST_TO_POINTER_NODISCARD                                                                                           \
    RETTR_NODISCARD_MSG("Ignoring the return value of cast_to_pointer<Ty>() might be an invalid call.")
#define RETTR_ANY_AS_NODISCARD RETTR_NODISCARD_MSG("Ignoring the return value of as<Ty>() might be an invalid call.")

namespace rettr {
    struct auto_deduce_t {
        explicit constexpr auto_deduce_t() = default;
    };

    constexpr inline auto_deduce_t auto_deduce{};
}

namespace rettr {
    /**
     * @brief 一个混合所有权的动态变量容器
     * @brief
     * 通常用于对不确定的变量或引用进行运算符或访问操作。同时，因其混合所有权的特性，在持有某个对象的时候，any将会采用RAII策略，反之则仅保留使用权，因此，any有大量功能不同于std::any
     */
    class any {
    public:
        friend class implements::any_reference<any>;

        template <typename... Handlers>
        using matcher = implements::any_matcher<any, Handlers...>;

        using iterator = implements::any_iterator_impl<any>;
        using const_iterator = iterator;
        using reference = implements::any_reference<any>;
        using const_reference = implements::any_reference<any>;

        constexpr any() noexcept {
        }

        RETTR_INLINE any(const any &right) {
            using namespace implements;
            if (!right.has_value()) {
                return;
            }
            switch (right.get_representation()) {
                case any_representation::_small:
                    right.storage.small_any_raii_manager->copy_(&storage.buffer, &right.storage.buffer);
                    storage.small_any_raii_manager = right.storage.small_any_raii_manager;
                    break;
                case any_representation::big:
                    storage.ptr = right.storage.big_any_raii_manager->copy_(right.storage.ptr);
                    storage.big_any_raii_manager = right.storage.big_any_raii_manager;
                    break;
                case any_representation::reference:
                    storage.ptr = right.storage.ptr;
                    break;
                default:
                    break;
            }
            storage.executer = right.storage.executer;
            storage.type_data = right.storage.type_data;
        }

        /**
         * @brief 将目标对象进行移动构造
         * @return 要移动的对象
         */
        RETTR_INLINE any(any &&right) noexcept {
            move_from(right);
        }

        template <
            typename ValueType,
            std::enable_if_t<std::conjunction_v<std::negation<helper::is_any_of<std::decay_t<ValueType>, any, reference>>,
                                                std::negation<is_specialization<std::decay_t<ValueType>, std::in_place_type_t>>,
                                                std::is_copy_constructible<ValueType>, std::negation<std::is_same<ValueType, any>>>,
                             int> = 0>
        any(ValueType &&value) { // NOLINT
            emplace_<ValueType>(std::forward<ValueType>(value));
        }

        template <
            typename ValueType, typename... Types,
            std::enable_if_t<std::is_constructible_v<ValueType, Types...> && !helper::is_any_of_v<ValueType, any, reference>, int> = 0>
        any(std::in_place_type_t<ValueType>, Types &&...args) { // NOLINT
            emplace_<ValueType>(std::forward<Types>(args)...);
        }

        template <typename ValueType, typename Elem, typename... Types,
                  std::enable_if_t<std::is_constructible_v<ValueType, std::initializer_list<Elem> &, Types...> &&
                                       !helper::is_any_of_v<ValueType, any, reference>,
                                   int> = 0>
        explicit any(std::in_place_type_t<ValueType>, std::initializer_list<Elem> ilist, Types &&...args) {
            emplace_<ValueType>(ilist, std::forward<Types>(args)...);
        }

        any(reference right) : any{right.construct_from_this()} { // NOLINT
        }

        RETTR_INLINE ~any() {
            reset();
        }

        any &operator=(const any &right) {
            any tmp = right;
            reset_and_move_from(tmp);
            return *this;
        }

        any &operator=(any &&right) noexcept {
            any tmp = std::move(right);
            reset_and_move_from(tmp);
            return *this;
        }

        any &operator=(reference ref) noexcept {
            any tmp = ref.construct_from_this();
            reset_and_move_from(tmp);
            return *this;
        }

        template <typename ValueType,
                  std::enable_if_t<!helper::is_any_of_v<std::decay_t<ValueType>, any, reference, const_reference>, int> = 0>
        any &operator=(ValueType &&value) {
            any tmp = std::forward<ValueType>(value);
            reset_and_move_from(tmp);
            return *this;
        }

        template <typename ValueType, typename... Types, std::enable_if_t<std::is_constructible_v<ValueType, Types...>, int> = 0>
        decltype(auto) emplace(Types &&...args) {
            reset();
            return emplace_<ValueType>(std::forward<Types>(args)...);
        }

        template <typename ValueType, typename Elem, typename... Types,
                  std::enable_if_t<std::is_constructible_v<ValueType, std::initializer_list<Elem> &, Types...>, int> = 0>
        decltype(auto) emplace(std::initializer_list<Elem> ilist, Types &&...args) {
            reset();
            return emplace_<ValueType>(ilist, std::forward<Types>(args)...);
        }

        void reset() {
            using namespace implements;
            if (!has_value()) {
                return;
            }
            switch (get_representation()) {
                case any_representation::_small:
                    storage.small_any_raii_manager->destroy_(&storage.buffer);
                    break;
                case any_representation::big:
                    storage.big_any_raii_manager->destory_(const_cast<void *>(storage.ptr));
                    break;
                case any_representation::reference:
                default:
                    break;
            }
            storage.type_data = 0;
        }

        void swap(any &right) noexcept {
            any tmp = std::move(*this);
            reset_and_move_from(right);
            right.reset_and_move_from(tmp);
        }

        RETTR_NODISCARD bool has_value() const noexcept {
            return storage.type_data != 0;
        }

        RETTR_NODISCARD const typeinfo &type() const noexcept {
            const typeinfo *const info = type_info();
            return info != nullptr ? *info : rettr_typeid(void);
        }

        RETTR_INLINE_NODISCARD typeinfo inner_decleartion_type(any_inner_declaertion query) const noexcept {
            typeinfo type;
            std::tuple tuple{query, &type};
            storage.executer->invoke(implements::any_operation::query_inner_declaertion_type, &tuple);
            return type;
        }

        template <typename Type>
        RETTR_ANY_AS_NODISCARD auto as() -> decltype(auto) {
            if (!is<Type>()) {
                if (!implements::is_as_runnable<Type>(type())) {
                    throw std::bad_cast();
                }
            }
            return implements::as_impl<Type>(target_as_void_ptr(), type());
        }

        template <typename Type>
        RETTR_ANY_AS_NODISCARD auto as() const -> decltype(auto) {
            using namespace helper;
            using ret_type = decltype(std::declval<any &>().template as<Type>());
            rettr_let nonconst = const_cast<any *>(this);
            if constexpr (std::is_rvalue_reference_v<ret_type>) {
                return nonconst->template as<helper::add_const_rvalue_ref_t<Type>>();
            } else {
                return nonconst->template as<helper::add_const_lvalue_ref_t<Type>>();
            }
        }

        template <typename Decayed>
        RETTR_ANY_CAST_TO_POINTER_NODISCARD rettr_fn cast_to_pointer() const noexcept -> std::add_pointer_t<
            std::add_const_t<std::conditional_t<std::is_reference_v<Decayed>, std::remove_reference_t<Decayed> *, Decayed *>>> {
            using namespace helper;
            using type = std::conditional_t<std::is_reference_v<Decayed>, std::remove_reference_t<Decayed>, Decayed>;
            if (!rettr::implements::is_as_runnable<type>(this->type())) {
                return nullptr;
            }
            return static_cast<const type *>(target_as_void_ptr());
        }

        template <typename Decayed>
        RETTR_ANY_CAST_TO_POINTER_NODISCARD rettr_fn cast_to_pointer() noexcept
            -> std::add_pointer_t<std::conditional_t<std::is_reference_v<Decayed>, std::remove_reference_t<Decayed>, Decayed>> {
            using namespace helper;
            using type = std::conditional_t<std::is_reference_v<Decayed>, std::remove_reference_t<Decayed>, Decayed>;
            if (!rettr::implements::is_as_runnable<type>(this->type())) {
                return nullptr;
            }
            return static_cast<type *>(const_cast<void *>(target_as_void_ptr()));
        }

        template <typename TargetType>
        any &transform() {
            if (is<TargetType>()) {
                any(std::in_place_type<TargetType>, this->template as<TargetType>()).swap(*this);
                return *this;
            }
            if (is_convertible<TargetType>()) {
                any(std::in_place_type<TargetType>, this->template convert<TargetType>()).swap(*this);
            }
            return *this;
        }

        template <typename Fx>
        any &transform(Fx &&handler) {
            using namespace helper;
            using type_list = function_argument_list<Fx>;
            using target_type = typename type_at<0, type_list>::type;
            if (is<target_type>()) {
                any(std::invoke(handler, this->template as<target_type>())).swap(*this);
            } else if (is_convertible<target_type>()) {
                any(std::invoke(handler, this->template convert<target_type>())).swap(*this);
            }
            return *this;
        }

        template <typename TargetType>
        RETTR_NODISCARD TargetType convert() {
            using rettr::traits;
            if (type().is_compatible(rettr_typeid(TargetType))) {
                return as<TargetType>();
            }
            if constexpr (is_any_convert_invocable<TargetType>) {
                return any_converter<TargetType>::basic_convert(this->target_as_void_ptr(), this->type());
            } else {
                throw std::bad_cast{};
            }
            std::terminate();
        }

        template <typename TargetType>
        RETTR_NODISCARD TargetType convert() const {
            return const_cast<any *>(this)->convert<TargetType>();
        }

        template <typename TargetType>
        RETTR_NODISCARD bool is_convertible() const noexcept {
            return is_any_convertible<TargetType>(this->type());
        }

        RETTR_NODISCARD bool is_convertible(const typeinfo &type) const noexcept {
            return is_convertible_to(this->type(), type);
        }

        RETTR_NODISCARD const void *target_as_void_ptr() const {
            const void *ptr = nullptr;
            switch (get_representation()) {
                case implements::any_representation::big:
                case implements::any_representation::reference:
                    ptr = reinterpret_cast<const void *>(storage.ptr);
                    break;
                case implements::any_representation::_small:
                    ptr = reinterpret_cast<const void *const>(&storage.buffer);
                    break;
            }
            return ptr;
        }

        friend bool operator<(const any &left, const any &right) {
            if (left.has_value() && right.has_value()) {
                std::tuple tuple{&left, &right, implements::any_compare_operation::less};
                return left.storage.executer->invoke(implements::any_operation::compare, &tuple);
            }
            return false;
        }

        friend bool operator<=(const any &left, const any &right) {
            if (left.has_value() && right.has_value()) {
                std::tuple tuple{&left, &right, implements::any_compare_operation::less_eq};
                return left.storage.executer->invoke(implements::any_operation::compare, &tuple);
            }
            return false;
        }

        friend bool operator==(const any &left, const any &right) {
            if (left.has_value() && right.has_value()) {
                std::tuple tuple{&left, &right, implements::any_compare_operation::eq};
                return left.storage.executer->invoke(implements::any_operation::compare, &tuple);
            }
            return false;
        }

        friend bool operator>=(const any &left, const any &right) {
            if (left.has_value() && right.has_value()) {
                std::tuple tuple{&left, &right, implements::any_compare_operation::greater_eq};
                return left.storage.executer->invoke(implements::any_operation::compare, &tuple);
            }
            return false;
        }

        friend bool operator>(const any &left, const any &right) {
            if (left.has_value() && right.has_value()) {
                std::tuple tuple{&left, &right, implements::any_compare_operation::greater};
                return left.storage.executer->invoke(implements::any_operation::compare, &tuple);
            }
            return false;
        }

        friend bool operator!=(const any &left, const any &right) {
            if (left.has_value() && right.has_value()) {
                return !(left == right);
            }
            return false;
        }

        friend any operator+(const any &left, const any &right) {
            any recv;
            std::tuple tuple{&left, &right, &recv};
            left.storage.executer->invoke(implements::any_operation::add, &tuple);
            return recv;
        }

        friend any operator-(const any &left, const any &right) {
            any recv;
            std::tuple tuple{&left, &right, &recv};
            left.storage.executer->invoke(implements::any_operation::subtract, &tuple);
            return recv;
        }

        friend any operator%(const any &left, const any &right) {
            any recv;
            std::tuple tuple{&left, &right, &recv};
            left.storage.executer->invoke(implements::any_operation::mod, &tuple);
            return recv;
        }

        friend any operator*(const any &left, const any &right) {
            any recv;
            std::tuple tuple{&left, &right, &recv};
            left.storage.executer->invoke(implements::any_operation::multiply, &tuple);
            return recv;
        }

        friend any operator/(const any &left, const any &right) {
            any recv;
            std::tuple tuple{&left, &right, &recv};
            left.storage.executer->invoke(implements::any_operation::divide, &tuple);
            return recv;
        }

        any operator--() {
            any recv;
            std::tuple tuple{this, &recv};
            storage.executer->invoke(implements::any_operation::decr_prefix, &tuple);
            return recv;
        }

        any operator++() {
            any recv;
            std::tuple tuple{this, &recv};
            storage.executer->invoke(implements::any_operation::incr_prefix, &tuple);
            return recv;
        }

        any operator++(int) {
            any recv;
            std::tuple tuple{this, &recv};
            storage.executer->invoke(implements::any_operation::incr_postfix, &tuple);
            return recv;
        }

        any operator--(int) {
            any recv;
            std::tuple tuple{this, &recv};
            storage.executer->invoke(implements::any_operation::decr_postfix, &tuple);
            return recv;
        }

        any operator*() const {
            any recv;
            std::tuple tuple{true, this, &recv};
            storage.executer->invoke(implements::any_operation::dereference, &tuple);
            return recv;
        }

        any &operator+=(const any &right) {
            std::tuple tuple{this, &right, this};
            storage.executer->invoke(implements::any_operation::add, &tuple);
            return *this;
        }

        any &operator-=(const any &right) {
            std::tuple tuple{this, &right, this};
            storage.executer->invoke(implements::any_operation::subtract, &tuple);
            return *this;
        }

        any &operator/=(const any &right) {
            std::tuple tuple{this, &right, this};
            storage.executer->invoke(implements::any_operation::divide, &tuple);
            return *this;
        }

        any &operator%=(const any &right) {
            std::tuple tuple{this, &right, this};
            storage.executer->invoke(implements::any_operation::mod, &tuple);
            return *this;
        }

        any &operator*=(const any &right) {
            std::tuple tuple{this, &right, this};
            storage.executer->invoke(implements::any_operation::multiply, &tuple);
            return *this;
        }

        reference operator[](std::size_t index) {
            reference ret;
            any the_index{std::in_place_type<std::size_t>, index};
            std::tuple tuple{false, this, &ret, &the_index};
            storage.executer->invoke(implements::any_operation::access_element, &tuple);
            return ret;
        }

        const_reference operator[](std::size_t index) const {
            const_reference ret;
            any the_index{std::in_place_type<std::size_t>, index};
            std::tuple tuple{true, this, &ret, &the_index};
            storage.executer->invoke(implements::any_operation::access_element, &tuple);
            return ret;
        }

        reference operator[](const any &key) {
            reference ret;
            std::tuple tuple{false, this, &ret, &key};
            storage.executer->invoke(implements::any_operation::access_element, &tuple);
            return ret;
        }

        const_reference operator[](const any &key) const {
            const_reference ret;
            std::tuple tuple{true, this, &ret, &key};
            storage.executer->invoke(implements::any_operation::access_element, &tuple);
            return ret;
        }

        RETTR_NODISCARD iterator begin() {
            iterator ret{};
            std::tuple tuple{false, this, &ret};
            storage.executer->invoke(implements::any_operation::container_begin, &tuple);
            return ret;
        }

        RETTR_NODISCARD const_iterator begin() const {
            const_iterator ret{};
            std::tuple tuple{true, this, &ret};
            storage.executer->invoke(implements::any_operation::container_begin, &tuple);
            return ret;
        }

        RETTR_NODISCARD iterator end() {
            iterator ret{};
            std::tuple tuple{false, this, &ret};
            storage.executer->invoke(implements::any_operation::container_end, &tuple);
            return ret;
        }

        RETTR_NODISCARD const_iterator end() const {
            const_iterator ret{};
            std::tuple tuple{true, this, &ret};
            storage.executer->invoke(implements::any_operation::container_end, &tuple);
            return ret;
        }

        /**
         * @brief 获取any存储的当前类型变量对应的哈希值
         * @attention 需当前类型支持计算哈希
         * @attention 优先通过std::hash求值，如果std::hash不可用，则使用rettr::hash作为哈希支持
         */
        RETTR_NODISCARD std::size_t hash_code() const noexcept {
            std::size_t ret{};
            std::tuple tuple{this, &ret};
            if (storage.executer->invoke(implements::any_operation::eval_hash, &tuple)) {
                return ret;
            }
            std::terminate();
        }

        template <typename... Fx>
        any match(Fx &&...funcs) const noexcept((function_traits<Fx>::is_noexcept && ...)) {
            return matcher<Fx...>{std::forward<Fx>(funcs)...}.invoke(*this);
        }

        template <typename Rx, typename... Fx>
        Rx match(Fx &&...funcs) const {
            return matcher<Fx...>{std::forward<Fx>(funcs)...}.invoke(*this).template convert<Rx>();
        }

        template <typename... Types, typename... Fx>
        std::variant<Types...> match_for(Fx &&...funcs) const {
            auto res = match(std::forward<Fx>(funcs)...);
            return implements::match_variant_helper<0, std::variant<Types...>, helper::type_list<Types...>>(res);
        }

        template <typename... Fx>
        auto match_for(auto_deduce_t, Fx &&...funcs) const -> std::variant<function_return_type<Fx>...> {
            using namespace std;
            static_assert((!std::is_void_v<function_return_type<Fx>> && ...), "Cannot accept a void-ret type functions.");
            using auto_deduce_type_list = helper::type_list<function_return_type<Fx>...>;
            using variant_type = typename helper::type_list_to_tuple_like<auto_deduce_type_list, std::variant>::type;
            auto res = match(std::forward<Fx>(funcs)...);
            return implements::match_variant_helper<0, variant_type, auto_deduce_type_list>(res);
        }

        template <typename Ty>
        bool destructure(Ty &&receiver) {
            return implements::destructure_impl<false>(this, storage.executer, std::forward<Ty>(receiver));
        }

        template <typename Ty>
        bool destructure(Ty &&receiver) const {
            return implements::destructure_impl<true>(this, storage.executer, std::forward<Ty>(receiver));
        }

        template <typename... Types>
        std::tuple<Types...> destructure() {
            std::tuple<Types...> ret = {};
            this->destructure(ret);
            return ret;
        }

        template <typename... Types>
        std::tuple<Types...> destructure() const {
            std::tuple<Types...> ret = {};
            this->destructure(ret);
            return ret;
        }

        template <typename CharType, typename Any, std::enable_if_t<std::is_same_v<std::decay_t<Any>, any>, int> = 0>
        friend std::basic_ostream<CharType> &operator<<(std::basic_ostream<CharType> &left, const Any &right) {
            if (!right.has_value()) {
                return left;
            }
            constexpr bool is_char = std::is_same_v<CharType, char>;
            std::tuple<std::basic_ostream<CharType> * /* ostream */, const any * /* any */> params{&left, &right};
            std::tuple<bool /* is_char/is_wchar_t */, void * /* params */> tuple{is_char, &params};
            if (const bool ok = right.storage.executer->invoke(implements::any_operation::output_any, &tuple); !ok) {
                left.setstate(std::ios::ios_base::failbit);
            }
            return left;
        }

        template <typename Type>
        RETTR_NODISCARD bool is() const noexcept {
            return type() == rettr_typeid(Type);
        }

        template <typename... Types>
        RETTR_NODISCARD bool is_one_of() const noexcept {
            return (is<Types>() || ...);
        }

        template <typename... Types>
        RETTR_NODISCARD bool is_one_of_convertible() const noexcept {
            return (is_convertible<Types>() || ...);
        }

        RETTR_NODISCARD bool has_ownership() const noexcept {
            return get_representation() != implements::any_representation::reference && has_value();
        }

        RETTR_NODISCARD reference as_reference() noexcept {
            return as_ref_impl<implements::any_operation::get_reference>();
        }

        RETTR_NODISCARD const_reference as_reference() const noexcept {
            return as_ref_impl<implements::any_operation::get_reference>();
        }

        RETTR_NODISCARD const_reference as_const_reference() const noexcept {
            return as_ref_impl<implements::any_operation::get_reference>();
        }

        RETTR_NODISCARD reference as_lvalue_reference() noexcept {
            return as_ref_impl<implements::any_operation::get_lvalue_reference>();
        }

        RETTR_NODISCARD const_reference as_lvalue_reference() const noexcept {
            return as_ref_impl<implements::any_operation::get_lvalue_reference>();
        }

        RETTR_NODISCARD const_reference as_const_lvalue_reference() const noexcept {
            return as_ref_impl<implements::any_operation::get_lvalue_reference>();
        }

        reference as_rvalue_reference() noexcept {
            return as_ref_impl<implements::any_operation::get_rvalue_reference>();
        }

        RETTR_NODISCARD const_reference as_rvalue_reference() const noexcept {
            return as_ref_impl<implements::any_operation::get_rvalue_reference>();
        }

        RETTR_NODISCARD const_reference as_const_rvalue_reference() const noexcept {
            return as_ref_impl<implements::any_operation::get_rvalue_reference>();
        }

        void swap_value(any &reference) {
            auto tuple = std::make_tuple(this, &reference);
            storage.executer->invoke(implements::any_operation::swap_value, &tuple);
        }

        const_iterator insert(const const_iterator &pos, const any &value) {
            const_iterator iterator;
            std::tuple tuple{this, &iterator, &pos, &value};
            storage.executer->invoke(implements::any_operation::container_insert_seq_like, &tuple);
            return iterator;
        }

        std::pair<const_iterator, bool> insert_as_maplike(const any &key) {
            std::pair<const_iterator, bool> ret_pair;
            std::tuple tuple{this, &ret_pair, &key, nullptr};
            storage.executer->invoke(implements::any_operation::container_insert_map_like, &tuple);
            return ret_pair;
        }

        std::pair<const_iterator, bool> insert_as_maplike(const any &key, const any &value) {
            std::pair<const_iterator, bool> ret_pair;
            std::tuple tuple{this, &ret_pair, &key, &value};
            storage.executer->invoke(implements::any_operation::container_insert_map_like, &tuple);
            return ret_pair;
        }

        void resize(const std::size_t new_size) {
            std::tuple tuple{this, new_size};
            storage.executer->invoke(implements::any_operation::container_resize, &tuple);
        }

        RETTR_NODISCARD std::size_t size() const {
            std::size_t size{};
            std::tuple tuple{this, &size};
            storage.executer->invoke(implements::any_operation::container_size, &tuple);
            return size;
        }

    private:
        static constexpr std::uintptr_t rep_mask = 3;

        template <implements::any_operation Operation>
        reference as_ref_impl() noexcept {
            reference reference;
            if (has_value()) {
                auto tuple = std::make_tuple(true, this, &reference);
                storage.executer->invoke(Operation, &tuple);
            }
            return reference;
        }

        template <implements::any_operation Operation>
        RETTR_NODISCARD const_reference as_ref_impl() const noexcept {
            const_reference reference;
            if (has_value()) {
                auto tuple = std::make_tuple(true, this, &reference);
                storage.executer->invoke(Operation, &tuple);
            }
            return reference;
        }

        RETTR_NODISCARD implements::any_representation get_representation() const noexcept {
            return static_cast<implements::any_representation>(storage.type_data & rep_mask);
        }

        RETTR_NODISCARD const typeinfo *type_info() const noexcept {
            return reinterpret_cast<const typeinfo *>(storage.type_data & ~rep_mask);
        }

        void move_from(any &right) noexcept {
            using namespace implements;
            storage.type_data = right.storage.type_data;
            storage.executer = right.storage.executer;
            switch (right.get_representation()) {
                case any_representation::_small:
                    storage.small_any_raii_manager = right.storage.small_any_raii_manager;
                    storage.small_any_raii_manager->move_(&storage.buffer, &right.storage.buffer);
                    break;
                case any_representation::big:
                    storage.big_any_raii_manager = right.storage.big_any_raii_manager;
                    storage.ptr = right.storage.ptr;
                    right.storage.type_data = 0;
                    break;
                case any_representation::reference:
                    storage.ptr = right.storage.ptr;
                    right.storage.type_data = 0;
                    break;
                default:
                    break;
            }
        }

        RETTR_INLINE void reset_and_move_from(any &right) noexcept {
            reset();
            move_from(right);
        }

        template <typename Decayed, typename... Types>
        RETTR_INLINE decltype(auto) emplace_(Types &&...args) {
            using namespace helper;
            using decayed = std::decay_t<Decayed>;
            if constexpr (std::is_reference_v<Decayed>) {
                using remove_reference = std::remove_reference_t<Decayed>;
                if constexpr (!std::is_array_v<remove_reference>) {
                    return emplace_ref<Decayed>(std::forward<Types>(args)...);
                } else {
                    return emplace_<decayed>(std::forward<Types>(args)...);
                }
            } else if constexpr (rettr::implements::any_is_small<decayed, sizeof(storage_t::buffer)>) {
                auto &object = reinterpret_cast<decayed &>(storage.buffer);
                storage.small_any_raii_manager = &rettr::implements::any_small_raii_manager_object<decayed>;
                ::new (std::addressof(object)) decayed(std::forward<Types>(args)...);
                storage.type_data = reinterpret_cast<std::uintptr_t>(&rettr_typeid(decayed)) |
                                    static_cast<std::uintptr_t>(rettr::implements::any_representation::_small);
                storage.executer = &rettr::implements::any_execution_policy_object<decayed, any>;
                return object;
            } else {
                rettr_let new_alloc_ptr = ::new decayed(std::forward<Types>(args)...);
                storage.ptr = new_alloc_ptr;
                storage.big_any_raii_manager = &rettr::implements::any_big_raii_manager_object<decayed>;
                storage.type_data = reinterpret_cast<std::uintptr_t>(&rettr_typeid(decayed)) |
                                    static_cast<std::uintptr_t>(rettr::implements::any_representation::big);
                storage.executer = &rettr::implements::any_execution_policy_object<decayed, any>;
                return *new_alloc_ptr;
            }
        }

        template <typename ReferenceType, typename Type>
        RETTR_INLINE decltype(auto) emplace_ref(Type &&reference) {
            storage.ptr = std::addressof(reference);
            storage.type_data = reinterpret_cast<std::uintptr_t>(&rettr_typeid(ReferenceType)) |
                                static_cast<std::uintptr_t>(implements::any_representation::reference);
            storage.executer = &implements::any_execution_policy_object<Type, any>;
            if constexpr (std::is_rvalue_reference_v<ReferenceType>) {
                return std::move(reference);
            } else {
                return reference;
            }
        }

        struct storage_t {
            union {
                const void *ptr;
                byte_t buffer[56];
            };
            union {
                const implements::big_any_raii_manager *big_any_raii_manager;
                const implements::small_any_raii_manager *small_any_raii_manager;
            };
            const implements::any_execution_policy *executer;
            std::uintptr_t type_data;
        };

        union {
            std::max_align_t dummy;
            storage_t storage{};
        };
    };
}

namespace rettr {
    template <typename Ty, typename... Args,
              std::enable_if_t<std::is_constructible_v<any, std::in_place_type_t<Ty>, Args...>, int> = 0>
    RETTR_NODISCARD any make_any(Args &&...args) {
        return any{std::in_place_type<Ty>, std::forward<Args>(args)...};
    }

    template <typename Ty, typename U, typename... Args,
              std::enable_if_t<std::is_constructible_v<any, std::in_place_type_t<Ty>, std::initializer_list<U> &, Args...>, int> = 0>
    RETTR_NODISCARD any make_any(std::initializer_list<U> initializer_list, Args &&...args) {
        return any{std::in_place_type<Ty>, initializer_list, std::forward<Args>(args)...};
    }

    RETTR_INLINE void swap(any &left, any &right) noexcept {
        left.swap(right);
    }
}

namespace rettr {
    template <typename TargetType>
    struct any_converter<TargetType, std::enable_if_t<std::is_reference_v<TargetType> || helper::is_pointer_reference_v<TargetType>>> {
        static decltype(auto) basic_convert(const void *target_pointer, const typeinfo &type) {
            return implements::as_impl<TargetType>(target_pointer, type);
        }

        static bool is_convertible(const typeinfo &type) {
            return implements::is_as_runnable<TargetType>(type);
        }
    };

    template <typename TargetType>
    struct any_converter<TargetType, std::enable_if_t<std::is_pointer_v<TargetType> || helper::is_pointer_reference_v<TargetType>>> {
        static rettr_fn basic_convert(const void *target_pointer, const typeinfo &type) -> TargetType {
            if (type.is_nullptr()) {
                return nullptr;
            }
            return implements::as_impl<TargetType>(target_pointer, type);
        }

        static bool is_convertible(const typeinfo &type) {
            if (type.is_nullptr()) {
                return true;
            }
            return implements::is_as_runnable<TargetType>(type);
        }
    };

    template <typename TargetType>
    struct any_converter<TargetType, std::enable_if_t<std::is_same_v<std::nullptr_t, std::remove_reference_t<TargetType>>>> {
        static rettr_fn basic_convert(const void *target_pointer, const typeinfo &type) -> TargetType {
            if (type.is_pointer() || type.is_nullptr()) {
                return nullptr;
            }
            throw std::bad_cast{};
        }

        static bool is_convertible(const typeinfo &type) {
            return type.is_pointer() || type.is_nullptr();
        }
    };

    template <typename TargetType>
    struct any_converter<TargetType, std::enable_if_t<std::is_arithmetic_v<TargetType>>>
        : enable_for_type_convert<any_converter<TargetType>> {
        static TargetType basic_convert(const void *target_pointer, const typeinfo &type) {
            switch (const typeinfo target_type = type; target_type.remove_cvref().hash_code()) {
                case rettr_typehash(int):
                    return static_cast<TargetType>(*static_cast<const int *>(target_pointer));
                case rettr_typehash(char):
                    return static_cast<TargetType>(*static_cast<const char *>(target_pointer));
                case rettr_typehash(float):
                    return static_cast<TargetType>(*static_cast<const float *>(target_pointer));
                case rettr_typehash(double):
                    return static_cast<TargetType>(*static_cast<const double *>(target_pointer));
                case rettr_typehash(long):
                    return static_cast<TargetType>(*static_cast<const long *>(target_pointer));
                case rettr_typehash(bool):
                    return static_cast<TargetType>(*static_cast<const bool *>(target_pointer));
                case rettr_typehash(long long):
                    return static_cast<TargetType>(*static_cast<const long long *>(target_pointer));
                case rettr_typehash(short):
                    return static_cast<TargetType>(*static_cast<const short *>(target_pointer));
                case rettr_typehash(std::int8_t):
                    return static_cast<TargetType>(*static_cast<const std::int8_t *>(target_pointer));
                case rettr_typehash(std::uint8_t):
                    return static_cast<TargetType>(*static_cast<const std::uint8_t *>(target_pointer));
                case rettr_typehash(std::uint16_t):
                    return static_cast<TargetType>(*static_cast<const std::uint16_t *>(target_pointer));
                case rettr_typehash(std::uint32_t):
                    return static_cast<TargetType>(*static_cast<const std::uint32_t *>(target_pointer));
                case rettr_typehash(std::uint64_t):
                    return static_cast<TargetType>(*static_cast<const std::uint64_t *>(target_pointer));
#if RETTR_USING_LINUX
                case rettr_typehash(long long unsigned int):
                    return static_cast<TargetType>(*static_cast<const long long unsigned int *>(target_pointer));
#endif
#if RETTR_USING_MACOS
                case rettr_typehash(unsigned long):
                    return static_cast<TargetType>(*static_cast<const unsigned long *>(target_pointer));
#endif
                default:
                    break;
            }
            throw std::bad_cast{};
        }

        static bool is_convertible(const typeinfo &type) {
            switch (const typeinfo target_type = type; target_type.remove_cvref().hash_code()) {
                case rettr_typehash(int):
                case rettr_typehash(char):
                case rettr_typehash(float):
                case rettr_typehash(double):
                case rettr_typehash(long):
                case rettr_typehash(bool):
                case rettr_typehash(long long):
                case rettr_typehash(short):
                case rettr_typehash(std::int8_t):
                case rettr_typehash(std::uint8_t):
                case rettr_typehash(std::uint16_t):
                case rettr_typehash(std::uint32_t):
                case rettr_typehash(std::uint64_t):
#if RETTR_USING_LINUX
                case rettr_typehash(long long unsigned int):
#endif
                    return true;
                default:
                    return false;
            }
        }
    };

    template <typename CharType, typename Traits>
    struct any_converter<std::basic_string_view<CharType, Traits>>
        : enable_for_type_convert<any_converter<std::basic_string_view<CharType, Traits>>> {
        using target_type = std::basic_string_view<CharType, Traits>;

        static target_type basic_convert(const void *target_pointer, const typeinfo &type) {
            using const_pointer = const CharType *;
            using pointer = CharType *;
            using pointer_to_const = const CharType *const;
            using same_type = std::basic_string_view<CharType, Traits>;
            using same_type_with_const = const std::basic_string_view<CharType, Traits>;
            using basic_string_t = std::basic_string<CharType, Traits>;
            using const_basic_string_t = const std::basic_string<CharType, Traits>;
            switch (type.remove_reference().hash_code()) {
                case rettr_typehash(const_pointer):
                case rettr_typehash(pointer):
                case rettr_typehash(pointer_to_const):
                    // return target_type{static_cast<const_pointer>(target_pointer)}; [[deprecated]]
                    return *static_cast<const const_pointer *>(target_pointer);
                case rettr_typehash(same_type):
                case rettr_typehash(same_type_with_const):
                    return *static_cast<const target_type *>(target_pointer);
                case rettr_typehash(basic_string_t):
                    RETTR_FALLTHROUGH;
                case rettr_typehash(const_basic_string_t): {
                    const basic_string_t &str = *static_cast<const basic_string_t *>(target_pointer);
                    return {str.data(), str.size()};
                }
                default:
                    break;
            }
            throw std::bad_cast{};
        }

        static bool is_convertible(const typeinfo &type) {
            switch (type.remove_reference().hash_code()) {
                case rettr_typehash(const CharType *):
                case rettr_typehash(CharType *):
                case rettr_typehash(const CharType *const):
                case rettr_typehash(std::basic_string_view<CharType>):
                case rettr_typehash(std::basic_string<CharType>):
                case rettr_typehash(const std::basic_string_view<CharType>):
                case rettr_typehash(const std::basic_string<CharType>):
                    return true;
                default:
                    break;
            }
            return false;
        }
    };

    template <typename CharType, typename Traits>
    struct any_converter<std::basic_string<CharType, Traits>>
        : enable_for_type_convert<any_converter<std::basic_string<CharType, Traits>>> {
        using target_type = std::basic_string<CharType, Traits>;

        static target_type basic_convert(const void *target_pointer, const typeinfo &type) {
            using const_pointer = const CharType *;
            using pointer = CharType *;
            using pointer_to_const = const CharType *const;
            using same_type = std::basic_string<CharType, Traits>;
            using same_type_with_const = const std::basic_string<CharType, Traits>;
            using basic_string_view_t = std::basic_string_view<CharType, Traits>;
            using const_basic_string_view_t = const std::basic_string_view<CharType, Traits>;
            switch (type.remove_reference().hash_code()) {
                case rettr_typehash(const_pointer):
                case rettr_typehash(pointer):
                case rettr_typehash(pointer_to_const):
                    // return target_type{static_cast<const_pointer>(target_pointer)}; [[deprecated]]
                    return *static_cast<const const_pointer *>(target_pointer);
                case rettr_typehash(same_type):
                case rettr_typehash(same_type_with_const):
                    return *static_cast<const target_type *>(target_pointer);
                case rettr_typehash(basic_string_view_t):
                    RETTR_FALLTHROUGH;
                case rettr_typehash(const_basic_string_view_t): {
                    const basic_string_view_t &str = *static_cast<const basic_string_view_t *>(target_pointer);
                    return {str.data(), str.size()};
                }
                default:
                    break;
            }
            throw std::bad_cast{};
        }

        static bool is_convertible(const typeinfo &type) {
            switch (type.remove_reference().hash_code()) {
                case rettr_typehash(const CharType *):
                case rettr_typehash(CharType *):
                case rettr_typehash(const CharType *const):
                case rettr_typehash(std::basic_string_view<CharType>):
                case rettr_typehash(std::basic_string<CharType>):
                case rettr_typehash(const std::basic_string_view<CharType>):
                case rettr_typehash(const std::basic_string<CharType>):
                    return true;
                default:
                    break;
            }
            return false;
        }
    };
}

namespace std { // NOLINT
    template <>
    struct hash<::rettr::any> { // NOLINT
        RETTR_NODISCARD std::size_t operator()(const ::rettr::any &right) const {
            return right.hash_code();
        }
    };
}

#if RETTR_HAS_CXX20
template <>
struct std::formatter<::rettr::any, char> {
    constexpr auto parse(auto &ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format specifier for rettr::any");
        }
        return it;
    }

    auto format(const ::rettr::any &val, auto &ctx) const {
        std::ostringstream ss;
        ss << val;
        for (auto c : ss.str()) {
            *ctx.out()++ = c;
        }
        return ctx.out();
    }
};

template <>
struct std::formatter<::rettr::any, wchar_t> {
    constexpr auto parse(auto &ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format specifier for rettr::any");
        }
        return it;
    }

    auto format(const ::rettr::any &val, auto &ctx) const {
        std::wostringstream ss;
        ss << val;
        for (auto c : ss.str()) {
            *ctx.out()++ = c;
        }
        return ctx.out();
    }
};
#endif

#if RETTR_USING_MSVC
#pragma warning(pop)
#endif

#undef RETTR_ANY_AS_NODISCARD
#undef RETTR_ANY_CAST_TO_POINTER_NODISCARD

#endif
