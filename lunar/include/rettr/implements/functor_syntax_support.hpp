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
#ifndef RETTR_IMPLEMENTS_FUNCTOR_SYNTAX_SUPPORT_HPP
#define RETTR_IMPLEMENTS_FUNCTOR_SYNTAX_SUPPORT_HPP

/*
 * @PrivateRemark：
 *
 * functor_syntax主要是需要给object建模提供字段属性的声明语法，若用户可以确保在取出某个对象不受到其歧义，该syntax可以帮助简化代码的实现在同一个写法下
 *
 *    字段访问：
 *
 *    rettr::object o = t.create_object();
 *    std::cout << o("field") << '\n';
 *
 *    o("field") = 100;
 *
 *    std::cout << o("field") << '\n';
 *
 *    函数调用：
 *    rettr::object o = t.create_object();
 *
 *    o("hello")();
 *
 */

#include <rettr/any.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/method.hpp>
#include <rettr/property.hpp>
#include <utility>
#include <variant>

#define RETTR_GENERATE_COMPARE_STUB(use_operator)                                                                                     \
    friend bool use_operator(const functor_operation &a, const functor_operation &b) {                                                      \
        return use_operator(std::get<1>(a.state)(a.object), std::get<1>(b.state)(b.object));                                          \
    }                                                                                                                                 \
                                                                                                                                      \
    friend bool use_operator(const functor_operation &a, const any &b) {                                                                 \
        return use_operator(std::get<1>(a.state)(a.object), b);                                                                       \
    }                                                                                                                                 \
    friend bool use_operator(const any &a, const functor_operation &b) {                                                                 \
        return use_operator(a, std::get<1>(b.state)(b.object));                                                                       \
    }

#define RETTR_GENERATE_BINARY_OPERATOR_STUB(use_operator, pred)                                                                       \
    friend any use_operator(const functor_operation &a, const functor_operation &b) {                                                       \
        return pred{}(std::get<1>(a.state)(a.object), std::get<1>(b.state)(b.object));                                                \
    }                                                                                                                                 \
                                                                                                                                      \
    friend any use_operator(const functor_operation &a, const any &b) {                                                                  \
        return pred{}(std::get<1>(a.state)(a.object), b);                                                                             \
    }                                                                                                                                 \
    friend any use_operator(const any &a, const functor_operation &b) {                                                                  \
        return pred{}(a, std::get<1>(b.state)(b.object));                                                                             \
    }

#define RETTR_GENERATE_COMPOUND_ARITHMETIC_OPERATOR_STUB(use_operator)                                                                \
    functor_operation &use_operator(const any &right) {                                                                                  \
        std::get<1>(this->state)(object).use_operator(right);                                                                         \
        return *this;                                                                                                                 \
    }

namespace rettr::implements {
    struct functor_syntax_ambiguous_state_t {
        constexpr explicit functor_syntax_ambiguous_state_t() noexcept = default;
    };

    constexpr functor_syntax_ambiguous_state_t functor_syntax_ambiguous_state;

    struct functor_prop_t {
        constexpr explicit functor_prop_t() noexcept = default;
    };

    constexpr functor_prop_t functor_reference;

    struct functor_method_t {
        constexpr explicit functor_method_t() noexcept = default;
    };

    constexpr functor_method_t functor_method;

    struct invalid_functor_t {
        constexpr explicit invalid_functor_t() noexcept = default;
    };

    constexpr invalid_functor_t invalid_functor;
}

namespace rettr::implements {
    class functor_operation {
    public:
        functor_operation(functor_prop_t, property prop, const object_view &object) noexcept :
            state{std::move(prop)}, object(object) {
        }

        functor_operation(functor_method_t, method meth, const object_view &object) noexcept :
            state{std::move(meth)}, object(object) {
        }

        functor_operation(functor_syntax_ambiguous_state_t) noexcept : state{functor_syntax_ambiguous_state} {
        }

        functor_operation(invalid_functor_t) noexcept : state{invalid_functor} {
        }

        RETTR_NODISCARD bool has_value() const noexcept {
            if (std::holds_alternative<invalid_functor_t>(state) ||
                std::holds_alternative<functor_syntax_ambiguous_state_t>(state)) {
                return false;
            }
            if (is_method()) {
                return !std::get<0>(state).empty();
            }
            return !std::get<1>(state).empty();
        }

        RETTR_NODISCARD bool is_method() const noexcept {
            return state.index() == 0;
        }

        RETTR_NODISCARD bool is_prop() const noexcept {
            return state.index() == 1;
        }

        template <typename... Args>
        any operator()(Args &&...args) {
            if (has_value() && !is_method()) {
                return {};
            }
            const auto &m = std::get<0>(state);
            if (m.is_static()) {
                return m.static_invoke(std::forward<Args>(args)...);
            }
            return m.invoke(object, std::forward<Args>(args)...);
        }

        template <typename CharType, typename FR, std::enable_if_t<std::is_same_v<std::decay_t<FR>, functor_operation>, int> = 0>
        friend std::basic_ostream<CharType> &operator<<(std::basic_ostream<CharType> &left, const FR &right) {
            if (right.has_value() && !right.is_prop()) {
                return left;
            }
            return left << static_cast<const rettr::any::reference &>(std::get<1>(right.state)(right.object));
        }

        template <typename ValueType>
        functor_operation &operator=(ValueType &&value) {
            if (is_prop()) {
                std::get<1>(state)(object) = std::forward<ValueType>(value);
            }
            return *this;
        }

        RETTR_GENERATE_COMPARE_STUB(operator<);
        RETTR_GENERATE_COMPARE_STUB(operator>);
        RETTR_GENERATE_COMPARE_STUB(operator==);
        RETTR_GENERATE_COMPARE_STUB(operator!=);
        RETTR_GENERATE_COMPARE_STUB(operator<=);
        RETTR_GENERATE_COMPARE_STUB(operator>=);

        RETTR_GENERATE_BINARY_OPERATOR_STUB(operator+, std::plus);
        RETTR_GENERATE_BINARY_OPERATOR_STUB(operator-, std::minus);
        RETTR_GENERATE_BINARY_OPERATOR_STUB(operator*, std::multiplies);
        RETTR_GENERATE_BINARY_OPERATOR_STUB(operator%, std::modulus);
        RETTR_GENERATE_BINARY_OPERATOR_STUB(operator/, std::divides);

        RETTR_GENERATE_COMPOUND_ARITHMETIC_OPERATOR_STUB(operator+=);
        RETTR_GENERATE_COMPOUND_ARITHMETIC_OPERATOR_STUB(operator-=);
        RETTR_GENERATE_COMPOUND_ARITHMETIC_OPERATOR_STUB(operator%=);
        RETTR_GENERATE_COMPOUND_ARITHMETIC_OPERATOR_STUB(operator*=);
        RETTR_GENERATE_COMPOUND_ARITHMETIC_OPERATOR_STUB(operator/=);

        template <typename... Fx>
        any match(Fx &&...funcs) const noexcept((function_traits<Fx>::is_noexcept && ...)) {
            return std::get<1>(state)(std::forward<Fx>(funcs)...);
        }

        template <typename Rx, typename... Fx>
        Rx match(Fx &&...funcs) const {
            return std::get<1>(state)(std::forward<Fx>(funcs)...);
        }

        template <typename... Fx>
        auto match_for(auto_deduce_t, Fx &&...funcs) const -> std::variant<function_return_type<Fx>...> {
            return std::get<1>(state)(object).match_for(auto_deduce, std::forward<Fx>(funcs)...);
        }

        template <typename... Types, typename... Fx>
        std::variant<Types...> match_for(Fx &&...funcs) const {
            return std::get<1>(state)(object).match_for(std::forward<Fx>(funcs)...);
        }

        template <typename Ty>
        bool destructure(Ty &&receiver) {
            return std::get<1>(state)(object).destructure(std::forward<Ty>(receiver));
        }

        template <typename Ty>
        bool destructure(Ty &&receiver) const {
            return std::get<1>(state)(object).destructure(std::forward<Ty>(receiver));
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

        RETTR_NODISCARD const rettr::method& method() const {
            return std::get<0>(state);
        }
        
        any::reference prop() {
            return std::get<1>(state)(object);
        }

    private:
        std::variant<rettr::method, property, functor_syntax_ambiguous_state_t, invalid_functor_t> state;
        object_view object{};
    };
}

#undef RETTR_GENERATE_COMPARE_STUB
#undef RETTR_GENERATE_BINARY_OPERATOR_STUB
#undef RETTR_GENERATE_COMPOUND_ARITHMETIC_OPERATOR_STUB

#endif
