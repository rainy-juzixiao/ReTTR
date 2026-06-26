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
#ifndef RETTR_FUNCTION_HPP
#define RETTR_FUNCTION_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/invocable/invoker_accessor.hpp>

namespace rettr::implements {
    template <typename Ty, typename... CtorArgs>
    class constructor_bind;

    template <typename Fx>
    class method_bind;
}

namespace rettr {
    class RETTR_API function {
    public:
        friend class object_view;
        friend class type;

        /**
         * @brief 构造一个空的反射函数对象。
         */
        function() noexcept;

        /**
         * @brief 从右值引用中进行移动构造。
         * @param right 待移动的函数对象。
         */
        function(function &&right) noexcept;

        /**
         * @brief 从左值常量引用中进行拷贝构造。
         * @param right 待拷贝的函数对象。
         */
        function(const function &right) noexcept;

        /**
         * @brief 与默认构造函数的行为相同。
         */
        function(std::nullptr_t) noexcept; // NOLINT

        /**
         * @brief 从给定的函数对象构造一个反射函数对象。
         * @tparam Fx 函数对象类型。
         * @param function 函数对象。
         */
        template <typename Fx, typename... Args, std::enable_if_t<function_traits<std::remove_reference_t<Fx>>::valid, int> = 0>
        function(Fx &&function, Args &&...default_arguments) noexcept : invoke_accessor_{} {
            // NOLINT
            using fx = std::remove_reference_t<Fx>;
            using traits = function_traits<fx>;
            using paramlist = typename traits::argument_list;

            static constexpr std::size_t arity = traits::arity;
            static constexpr std::size_t default_arg_count = sizeof...(Args);
            static constexpr std::size_t start_index = arity - default_arg_count;

            static_assert(default_arg_count <= arity, "Too many default arguments provided for the function.");
            static_assert(implements::check_default_args_compatibility<paramlist, start_index, Args...>(),
                          "Default arguments are not compatible with corresponding function parameters.");

            using implemented_type = typename implements::get_ia_implement_type<fx, implements::default_arguments_store<Args...>,
                                                                                function_traits<fx>>::type;
            if constexpr (sizeof(implemented_type) >= implements::fn_obj_soo_buffer_size) {
                invoke_accessor_ = ::new implemented_type(std::forward<Fx>(function), std::forward<Args>(default_arguments)...);
            } else {
                invoke_accessor_ = ::new (reinterpret_cast<implemented_type *>(invoker_storage))
                    implemented_type(std::forward<Fx>(function), std::forward<Args>(default_arguments)...);
            }
        }

        ~function();

        /**
         * @brief 调用函数，以静态方式，并返回结果。
         * @tparam Args 任意数量的函数参数类型，但是其数量需要与目标调用的参数数量一致。
         * @param args 任意数量的函数实参，需要与目标调用的参数数量一致。
         * @return 函数调用结果，以any形式。
         */
        template <typename... Args>
        RETTR_INLINE any static_invoke(Args &&...args) const {
            return invoke(non_exists_instance, std::forward<Args>(args)...);
        }

        /**
         * @brief 调用函数，并返回结果。
         * @tparam Args 任意数量的函数参数类型，但是其数量需要与目标调用的参数数量一致。
         * @param instance 对象实例。
         * @param args 任意数量的函数实参，需要与目标调用的参数数量一致。
         * @return 函数调用结果，以any形式。
         */
        template <typename... Args>
        RETTR_INLINE any invoke(object_view instance, Args &&...args) const {
            if (empty()) {
                throw rettr::bad_function_call();
            }
#if RETTR_ENABLE_DEBUG
            if (instance.type().is_const()) {
                if (!is_const()) {
                    errno = ECANCELED;
                    return {};
                }
            } else if (instance.type().has_traits(traits::is_volatile)) {
                if (!is_volatile()) {
                    errno = ECANCELED;
                    return {};
                }
            } else if (instance.type().is_rvalue_reference()) {
                if (!is_invoke_for_rvalue()) {
                    errno = ECANCELED;
                    return {};
                }
            }
#endif
            if constexpr (sizeof...(Args) == 0) {
                return invoke_accessor()->invoke(instance);
            } else {
                return invoke_accessor()->invoke(instance, implements::arg_store{std::forward<Args>(args)...});
            }
        }

        any invoke_variadic(object_view instance, array_range<class any> args = {}) const; // NOLINT

        /**
         * @brief 重载函数调用运算符，以调用函数并返回结果。
         * @tparam Args 任意数量的函数参数类型，但是其数量需要与目标调用的参数数量一致。
         * @param instance 对象实例。
         * @param args 任意数量的函数实参，需要与目标调用的参数数量一致。
         * @return 函数调用结果，以any形式。
         */
        template <typename... Args>
        RETTR_INLINE any operator()(object_view instance, Args &&...args) const {
            return invoke(instance, std::forward<Args>(args)...);
        }

        /**
         * @brief 检查当前反射函数对象是否为空。
         * @return 如果为空，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool empty() const noexcept;

        /**
         * @brief 从常量左值引用中拷贝函数对象。
         * @param right 从此对象拷贝函数对象。
         */
        void copy_from_other(const function &right) noexcept;

        /**
         * @brief 从右值引用中移动函数对象。
         * @param right 从此对象移动函数对象。
         */
        void move_from_other(function &&right) noexcept;

        /**
         * @brief 交换两个函数对象。
         * @param right 另一个函数对象。
         */
        void swap(function &right) noexcept;

        /**
         * @brief 从常量左值引用中拷贝函数对象。
         * @param right 从此对象拷贝函数对象。
         * @return 返回对象本身的引用。
         */
        function &operator=(const function &right) noexcept;

        /**
         * @brief 通过移动语义转移函数对象。
         * @param right 从此对象移动函数对象。
         * @return 返回对象本身的引用。
         */
        function &operator=(function &&right) noexcept;

        /**
         * @brief 将函数对象设置为空。
         * @return 返回对象本身的引用。
         */
        function &operator=(std::nullptr_t) noexcept;

        /**
         * @brief 获取函数对象的函数签名。
         * @return 返回函数签名。
         */
        RETTR_NODISCARD const typeinfo &function_signature() const noexcept;

        /**
         * @brief 获取函数对象的函数类型。
         * @return 返回函数类型。
         */
        RETTR_NODISCARD method_flags type() const noexcept;

        /**
         * @brief 获取函数对象的函数类型。检查是否具有特定的属性
         */
        RETTR_NODISCARD bool has(method_flags flag) const noexcept;

        /**
         * @brief 检查当前函数对象是否有效。
         * @return 如果有效，则返回true；否则返回false。
         */
        explicit operator bool() const noexcept;

        /**
         * @brief 检查当前函数对象是否与另一个函数对象相同。
         * @param right 另一个函数对象。
         * @return 如果相同，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool equal_with(const function &right) const noexcept;

        /**
         * @brief 检查当前函数对象是否与另一个函数对象不同。
         * @param right 另一个函数对象。
         * @return 如果不同，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool not_equal_with(const function &right) const noexcept;

        /**
         * @brief 清空函数对象。
         */
        void reset() noexcept;

        /**
         * @brief 重新绑定函数对象。
         * @param function 待绑定的函数对象。
         * @remark 以移动语义的方式绑定函数对象。
         */
        void rebind(function &&function) noexcept;

        /**
         * @brief 重新绑定函数对象。
         * @param function 待绑定的函数对象。
         * @remark 以拷贝语义的方式绑定函数对象。
         */
        void rebind(const function &function) noexcept;

        /**
         * @brief 重新绑定函数对象。
         * @remark 将函数对象设置为空。
         */
        void rebind(std::nullptr_t) noexcept;

        /**
         * @brief 获取函数对象的所属类。
         * @return 返回函数对象的所属类。
         */
        RETTR_NODISCARD const typeinfo &which_belongs() const noexcept;

        /**
         * @brief 获取函数对象的返回类型。
         * @return 返回函数对象的返回类型。
         */
        RETTR_NODISCARD const typeinfo &return_type() const noexcept;

        /**
         * @brief 获取函数对象的参数类型列表。
         * @return 返回函数对象的参数类型列表。
         */
        RETTR_NODISCARD const array_range<typeinfo> &paramlists() const noexcept;

        /**
         * @brief 获取函数对象的所需参数数量。
         * @return 返回函数对象的所需参数数量。
         */
        RETTR_NODISCARD std::size_t arity() const noexcept;

        /**
         * @brief 获取当前function对象中，从0开始索引的参数列表中的某个参数的类型信息
         * @param idx 索引
         * @return 返回参数类型信息
         */
        RETTR_NODISCARD const typeinfo &arg(std::size_t idx) const;

        /**
         * @brief 检查当前函数对象是否为静态函数。
         * @return 如果是静态函数，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool is_static() const noexcept;

        /**
         * @brief 检查当前函数对象是否为成员实例函数。
         * @return 如果是成员实例函数，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool is_memfn() const noexcept;

        /**
         * @brief 检查当前函数对象是否为const函数。
         * @return 如果是const函数，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool is_const() const noexcept;

        /**
         * @brief 检查当前函数对象是否为noexcept函数。
         * @return 如果是noexcept函数，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool is_noexcept() const noexcept;

        /**
         * @brief 检查当前函数对象是否为volatile函数。
         * @return 如果是volatile函数，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool is_volatile() const noexcept;

        /**
         * @brief 检查当前函数对象是否支持lvalue调用。
         * @return 如果支持lvalue调用，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool is_invoke_for_lvalue() const noexcept;

        /**
         * @brief 检查当前函数对象是否支持rvalue调用。
         * @return 如果支持rvalue调用，则返回true；否则返回false。
         */
        RETTR_NODISCARD bool is_invoke_for_rvalue() const noexcept;

        /**
         * @brief 检查当前函数对象是否可以调用给定的参数列表。
         * @param paramlist 参数列表。
         * @return 如果可以调用，则返回true；否则返回false。
         */
        RETTR_NODISCARD invocable_result is_invocable(array_range<typeinfo> paramlist) const noexcept;

        /**
         * @brief 检查当前函数对象是否可以调用给定的参数类型。
         * @tparam Args 参数类型。
         * @return 如果可以调用，则返回true；否则返回false。
         */
        template <typename... Args>
        RETTR_NODISCARD invocable_result is_invocable() const noexcept {
            if (empty()) {
                return invocable_result::failed;
            }
            if constexpr (sizeof...(Args) == 0) {
                return invoke_accessor()->is_invocable({});
            } else {
                static std::array<typeinfo, sizeof...(Args)> paramlist = {typeinfo::create<Args>()...};
                return is_invocable(paramlist);
            }
        }

        template <typename... Args>
        RETTR_NODISCARD invocable_result is_invocable_with(Args &&...args) const noexcept {
            implements::make_paramlist paramlist{std::forward<Args>(args)...};
            return is_invocable(paramlist.get());
        }

        RETTR_NODISCARD invocable_result is_variadic_invocable_with(array_range<any> args) const;

        /**
         * @brief 获取函数对象的目标函数指针。
         * @tparam Fx 目标函数类型。
         * @return 返回目标函数指针。
         */
        template <typename Fx>
        RETTR_NODISCARD Fx *target() const noexcept {
            assert(!empty() && "You're trying to get the arg count of a empty object!");
            if constexpr (std::is_same_v<Fx, function>) {
                return reinterpret_cast<function *>(invoke_accessor()->target(rettr_typeid(Fx)));
            } else {
                auto ptr = reinterpret_cast<Fx *>(invoke_accessor()->target(rettr_typeid(Fx)));
                return ptr;
            }
        }

        /**
         * @brief 比较两个函数对象是否相等。
         * @param left 左侧函数对象。
         * @param right 右侧函数对象。
         * @return 如果相等，则返回true；否则返回false。
         */
        friend bool operator==(const function &left, const function &right) noexcept {
            return left.equal_with(right);
        }

        /**
         * @brief 比较两个函数对象是否不相等。
         * @param left 左侧函数对象。
         * @param right 右侧函数对象。
         * @return 如果不相等，则返回true；否则返回false。
         */
        friend bool operator!=(const function &left, const function &right) noexcept {
            return left.not_equal_with(right);
        }

        /**
         * @brief 将函数对象的信息输出到流中。
         * @param os 输出流。
         * @param f 函数对象。
         * @return 返回输出流。
         */
        friend std::ostream &operator<<(std::ostream &os, const function &f) {
            if (f.empty()) {
                return os;
            }
            return os << f.function_signature().name() << " -> " << f.target<function *>();
        }

    private:
        template <typename Ty, typename... CtorArgs>
        friend class implements::constructor_bind;

        template <typename Ty>
        friend class implements::method_bind;

        RETTR_NODISCARD RETTR_INLINE implements::invoker_accessor *invoke_accessor() const noexcept {
            return invoke_accessor_;
        }

        RETTR_NODISCARD bool is_local() const noexcept;

        alignas(std::max_align_t) byte_t
            invoker_storage[implements::fn_obj_soo_buffer_size]{}; // 不使用std::array/std::aligned_storage
        implements::invoker_accessor *invoke_accessor_{nullptr};
    };

    /**
     * @brief 创建一个反射函数对象。
     * @tparam Fx 函数对象类型。
     * @param fx 函数对象。
     * @return 返回创建的反射函数对象。
     */
    template <typename Fx, std::enable_if_t<std::is_constructible_v<function, Fx>, int> = 0>
    function make_function(Fx &&fx) noexcept {
        return function{std::forward<Fx>(fx)};
    }

    /**
     * @brief 调用函数对象。
     * @tparam Args 函数参数类型。
     * @param fn 函数对象。
     * @param instance 对象实例。
     * @param args 函数实参。
     * @return 函数调用结果。
     */
    template <typename... Args>
    any invoke(const function &fn, object_view instance, Args &&...args) {
        return fn.invoke(instance, std::forward<Args>(args)...);
    }
}

#endif
