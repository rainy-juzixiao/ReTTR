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
#ifndef RETTR_MOON_TUPLE_LIKE_HPP
#define RETTR_MOON_TUPLE_LIKE_HPP

// NOLINTBEGIN, clang-format off

#include <functional>
#include <rettr/core/marco_gen.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/core/basic_constexpr_string.hpp>
#include <rettr/typeinfo.hpp>
#include <tuple>
#include <variant>

// NOLINTEND, clang-format on

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <rettr/annotations/moon.hpp>
#endif

/*
[N4849 13.7.5 Class template partial specializations(temp.class.spec) - 10]:
    The usual access checking rules do not apply to non-dependent names used to specify template arguments of the simple-template-id of
the partial specialization. [Note: The template arguments may be private types or objects that would normally not be accessible.
Dependent names cannot be checked when declaring the partial specialization, but will be checked when substituting into the partial
specialization. —end note]

通常的访问检查规则不适用于用于指定显式实例化的名称。
[注意：
特别是，函数声明符中使用的模板参数和名称（包括参数类型、返回类型和异常规范）可能是通常无法访问的私有类型或对象。
而模板可能是通常无法访问的成员模板或成员函数。
-结束注释]
*/
namespace rettr::implements {
    template <typename Ty>
    struct private_access_tag_t {};

    template <typename Class, auto... Fields>
    struct private_access {
        friend inline constexpr auto get_private_ptrs(const private_access_tag_t<Class> &) {
            return std::make_tuple(Fields...);
        }
    };

    template <typename Ty>
    constexpr private_access_tag_t<Ty> private_access_tag = {};
}

namespace rettr::implements {
    template <typename Ty, typename = void>
    struct has_get_private_ptrs : std::false_type {};

    template <typename Ty>
    struct has_get_private_ptrs<Ty, std::void_t<decltype(get_private_ptrs(private_access_tag<helper::remove_cvref_t<Ty>>))>>
        : std::true_type {};

    template <typename Ty, typename = void>
    struct get_private_ptrs_helper {
        static constexpr auto value = std::make_tuple();
    };

    template <typename Ty>
    struct get_private_ptrs_helper<Ty, std::void_t<decltype(get_private_ptrs(private_access_tag<helper::remove_cvref_t<Ty>>))>> {
        static constexpr auto value = get_private_ptrs(private_access_tag<helper::remove_cvref_t<Ty>>);
    };
}

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::implements {
    template <typename Ty>
    static constexpr auto member_array = []() consteval {
        using type = helper::remove_cvref_t<Ty>;
        constexpr auto ctx = std::meta::access_context::current();
        return std::define_static_array(std::meta::nonstatic_data_members_of(^^type, ctx));
    }();

    template <typename Ty>
    static constexpr auto unchecked_all_member_array = []() consteval {
        using type = helper::remove_cvref_t<Ty>;
        constexpr auto ctx = std::meta::access_context::unchecked();
        return std::define_static_array(std::meta::nonstatic_data_members_of(^^type, ctx));
    }();
}

namespace rettr::implements {
    template <typename Ty>
    constexpr rettr_fn members_count_impl() -> std::size_t { // as-if from fallback version
        if constexpr (std::is_aggregate_v<Ty>) {
            std::size_t count{0};
            template for (constexpr auto mem: member_array<Ty>) {
                if constexpr (constexpr auto attn = annotations::make_member_anno(mem);
                              !attn.template has<annotations::moon::ignore_tag>()) {
                    ++count;
                }
            }
            return count;
        } else {
            return 0;
        }
    }
}

#else

namespace rettr::implements {
    struct any_type {
        template <typename Ty>
        constexpr operator Ty() const {
            return {};
        }
    };

    template <typename Ty, typename... Args>
    constexpr rettr_fn test_constructible(int) -> decltype(Ty{std::declval<Args>()..., any_type{}}, std::true_type{}) {
        std::terminate();
        return {};
    }

    template <typename Ty, typename... Args>
    constexpr rettr_fn test_constructible(...) -> std::false_type {
        std::terminate();
        return {};
    }

    template <typename Ty, typename... Args>
    constexpr bool can_construct_with_one_more = decltype(test_constructible<Ty, Args...>(0))::value;

    template <typename Ty, typename... Args>
    constexpr std::size_t members_count_impl() {
        if constexpr (std::is_aggregate_v<Ty>) {
            if constexpr (can_construct_with_one_more<Ty, Args...>) {
                return members_count_impl<Ty, Args..., any_type>();
            } else {
                return sizeof...(Args);
            }
        } else {
            return 0;
        }
    }
}
#endif

namespace rettr {
    template <typename Ty, typename = void>
    struct reflectet_for_type {
        static constexpr bool invalid_mark = true;

        static constexpr inline std::size_t count = 0;

        static constexpr rettr_fn make() noexcept -> std::tuple<> {
            return std::make_tuple();
        }

        static constexpr rettr_fn bind_obj(Ty &) noexcept -> std::tuple<> {
            return std::make_tuple();
        }

        static constexpr rettr_fn member_names() noexcept -> auto {
            std::array<std::string_view, 0> empty;
            return empty;
        }
    };

    template <typename Type, typename = void>
    RETTR_CONSTEXPR_BOOL is_reflectet_for_type_valid = true;

    template <typename Type>
    RETTR_CONSTEXPR_BOOL is_reflectet_for_type_valid<Type, std::void_t<decltype(reflectet_for_type<Type>::invalid_mark)>> = false;
}

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::implements {
    template <std::size_t, typename Ty>
    struct refl_to_tuple_impl {
        static constexpr rettr_fn make() noexcept -> auto {
            using namespace std::meta;
            auto &fake = helper::get_fake_object<helper::remove_cvref_t<Ty>>();
            constexpr auto filtered = []() consteval {
                std::vector<info> result;
                template for (constexpr auto m: member_array<Ty>) {
                    if constexpr (constexpr auto anno = annotations::make_member_anno(m);
                                  !anno.template has<annotations::moon::ignore_tag>()) {
                        result.push_back(m);
                    }
                }
                return std::define_static_array(result);
            }();
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(&(fake.[:filtered[Is]:])...);
            }(std::make_index_sequence<filtered.size()>{});
        }

        static constexpr rettr_fn get_memptr_tuple() noexcept -> auto {
            using namespace std::meta;
            constexpr auto filtered = []() consteval {
                std::vector<info> result;
                template for (constexpr auto m: member_array<Ty>) {
                    constexpr auto anno = annotations::make_member_anno(m);
                    if constexpr (!anno.template has<annotations::moon::ignore_tag>()) {
                        result.push_back(m);
                    }
                }
                return std::define_static_array(result);
            }();
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(&[:filtered[Is]:]...);
            }(std::make_index_sequence<filtered.size()>{});
        }

        template <typename UTy, std::enable_if_t<std::is_same_v<helper::remove_cvref_t<UTy>, Ty>, int> = 0>
        static constexpr rettr_fn make_ptr(UTy &&obj) noexcept -> auto {
            constexpr auto pmembers = get_memptr_tuple();
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(&(obj.*std::get<Is>(pmembers))...);
            }(std::make_index_sequence<std::tuple_size_v<helper::remove_cvref_t<decltype(pmembers)>>>{});
        }

        using type = decltype(make());
    };
}

#else

namespace rettr::implements {
    template <std::size_t N, typename Ty>
    struct refl_to_tuple_impl;

    template <typename Ty>
    struct refl_to_tuple_impl<0, Ty> : std::integral_constant<std::size_t, 0> {
        static constexpr rettr_fn make() noexcept -> std::tuple<> {
            return std::make_tuple();
        };

        using type = decltype(make());
    };
}

#define RETTR_DECLARE_TO_TUPLE(N)                                                                                                     \
    template <typename Ty>                                                                                                            \
    struct rettr::implements::refl_to_tuple_impl<N, Ty> : std::integral_constant<std::size_t, N> {                                    \
        static constexpr rettr_fn make() noexcept -> auto {                                                                           \
            auto &[RETTR_TO_TUPLE_EXPAND_ARGS(N)] = helper::get_fake_object<helper::remove_cvref_t<Ty>>();                            \
            auto ref_tup = std::tie(RETTR_TO_TUPLE_EXPAND_ARGS(N));                                                                   \
            auto get_ptrs = [](auto &..._refs) { return std::make_tuple(&_refs...); };                                                \
            return std::apply(get_ptrs, ref_tup);                                                                                     \
        }                                                                                                                             \
        template <typename UTy, std::enable_if_t<std::is_same_v<helper::remove_cvref_t<UTy>, Ty>, int> = 0>                           \
        static constexpr rettr_fn make_ptr(UTy &&obj) noexcept -> auto {                                                              \
            auto &[RETTR_TO_TUPLE_EXPAND_ARGS(N)] = obj;                                                                              \
            auto ref_tup = std::tie(RETTR_TO_TUPLE_EXPAND_ARGS(N));                                                                   \
            auto get_ptrs = [](auto &..._refs) { return std::make_tuple(&_refs...); };                                                \
            return std::apply(get_ptrs, ref_tup);                                                                                     \
        }                                                                                                                             \
        using type = decltype(make());                                                                                                \
    };

RETTR_GENERATE_MACRO_FOR_256(RETTR_DECLARE_TO_TUPLE)

#undef RETTR_DECLARE_TO_TUPLE

#endif

namespace rettr {
    /**
     * @brief 获取聚合类的成员个数。
     * @tparam Ty 要获取的对应类型
     * @attention 如果没有定义任何注册宏或是该类并非聚合类的时候，则无法获取
     * @remark
     * 默认尝试从聚合类中获取大小，如果特化了reflectet_for_type，即is_reflectet_for_type_valid<helper::remove_cvref_t<Ty>>表达式结果为true时
     * @remark 将返回该特化指定的大小，另外，RETTR_REFLECT_TUPLE_LIKE和RETTR_PRIVATE_REFLECT_TUPLE_LIKE定义的注册也同样适用
     */
    template <typename Ty>
    struct member_count {
        static constexpr rettr_fn eval() noexcept -> std::size_t {
            if constexpr (is_reflectet_for_type_valid<helper::remove_cvref_t<Ty>>) {
                return reflectet_for_type<helper::remove_cvref_t<Ty>>::count;
            } else {
                return implements::members_count_impl<helper::remove_cvref_t<Ty>>();
            }
        }

        static constexpr std::size_t value = eval();
    };

    template <typename Ty>
    static inline constexpr std::size_t member_count_v = member_count<Ty>::value;

    template <typename Ty>
    constexpr rettr_fn struct_to_tuple() -> auto {
        constexpr std::size_t count = member_count_v<helper::remove_cvref_t<Ty>>;
        if constexpr (is_reflectet_for_type_valid<Ty>) {
            return reflectet_for_type<Ty>::make();
        } else if constexpr (count != 0 && std::is_aggregate_v<Ty>) {
            return implements::refl_to_tuple_impl<count, Ty>::make();
        } else {
            return std::make_tuple();
        }
    }

    template <std::size_t Idx, typename Ty>
    struct tuple_element {
        using type = std::tuple_element_t<Idx, decltype(struct_to_tuple<Ty>())>;
    };

    template <std::size_t Idx, typename Ty>
    using tuple_element_t = std::tuple_element_t<Idx, decltype(struct_to_tuple<helper::remove_cvref_t<Ty>>())>;
}

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::implements {
    template <typename Ty, std::size_t Size>
    RETTR_CONSTEVAL rettr_fn get_member_names_compositor(std::array<std::string_view, Size> &array) noexcept -> void {
        using namespace std::meta;
        auto &fake = helper::get_fake_object<Ty>();
        auto tp = struct_to_tuple<Ty>();

        [&]<std::size_t... Idx>(std::index_sequence<Idx...>) consteval {
            (
                [&]() consteval {
                    template for (constexpr auto m: unchecked_all_member_array<Ty>) {
                        if (static_cast<void *>(std::addressof(fake.[:m:])) == static_cast<void const *>(std::get<Idx>(tp))) {
                            try_apply_rename<m>(array[Idx]);
                            try_apply_name_style<Ty, m>(array[Idx]);
                            try_apply_prefix_and_suffix_tag<Ty, m>(array[Idx]);
                            break;
                        }
                    }
                }(),
                ...);
        }(std::make_index_sequence<Size>{});
    }
}

#endif

#if RETTR_USING_MSVC

namespace rettr::implements { // workaround: 确保member的名称在MSVC投射正确
    template <typename Ty>
    struct wrapper {
        using Type = Ty;

        Ty v;
    };

    template <typename Ty>
    wrapper(Ty) -> wrapper<Ty>;

    template <typename Ty>
    inline constexpr auto wrap(const Ty &arg) noexcept {
        return wrapper{arg};
    }
}

#endif

namespace rettr {
    /**
     * @brief 尝试获取指定类型中所有成员的名称
     * @tparam Ty 要获取的对应类型
     * @remark 如果使用RETTR_REFLECT_TUPLE_LIKE或RETTR_PRIVATE_REFLECT_TUPLE_LIKE注册了一个类型，那么此方法将返回注册宏中指定的成员名称
     * @return 返回指定类型中所有成员的名称
     *
     * @attention get_member_names以及其依赖的函数在IDE中可能会导致错误的结果在constexpr求值中，但实际编译期求值会得到正确的结果
     * @attention 因此，如果对IDE中的结果有洁癖，请尽可能避免获取字符串，除非，你定义了注册，这样才能确保IDE生成正确的结果
     */
    template <typename Ty>
    RETTR_CONSTEVAL rettr_fn get_member_names() noexcept -> auto {
        using type = helper::remove_cvref_t<Ty>;
        if constexpr (is_reflectet_for_type_valid<type>) {
            auto array = reflectet_for_type<type>::member_names();
#if RETTR_HAS_CXX26_STATIC_REFLECTION
            implements::get_member_names_compositor<Ty, reflectet_for_type<type>::count>(array);
#endif
            return array;
        } else {
#if RETTR_HAS_CXX20
            constexpr bool has_get_private_ptrs_v = implements::has_get_private_ptrs<type>::value;
            static_assert(member_count_v<type> != 0 || has_get_private_ptrs_v || std::is_empty_v<type>, "Failed!");
            if constexpr (std::is_empty_v<type>) {
                return std::array<std::string_view, 0>{};
            } else if constexpr (!has_get_private_ptrs_v) {
                constexpr size_t count = member_count_v<type>;
                std::array<std::string_view, count> array{}; // 创建对应的数组
                constexpr auto tp = struct_to_tuple<type>();
#if RETTR_USING_MSVC
                [&array, &tp]<std::size_t... I>(std::index_sequence<I...>) mutable {
                    ((array[I] = foundation::ctti::variable_name<implements::wrap(std::get<I>(tp))>()), ...);
                }(std::make_index_sequence<member_count_v<Ty>>{});
#else
                [&array, &tp]<std::size_t... I>(std::index_sequence<I...>) mutable {
                    ((array[I] = variable_name<(std::get<I>(tp))>()), ...);
                }(std::make_index_sequence<member_count_v<Ty>>{});
#endif
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
                implements::get_member_names_compositor<type, count>(array);
#endif
                return array;
            } else {
                constexpr auto tp = implements::get_private_ptrs_helper<type>::value; // 使用 helper 获取 `tp`
                constexpr std::size_t tuple_size = std::tuple_size_v<decltype(tp)>;
                std::array<std::string_view, tuple_size> array{};
#if RETTR_USING_MSVC
                [&array, &tp]<std::size_t... I>(std::index_sequence<I...>) mutable {
                    ((array[I] = foundation::ctti::variable_name<implements::wrap(std::get<I>(tp))>()), ...);
                }(std::make_index_sequence<tuple_size>{});
#else
                [&array, &tp]<std::size_t... I>(std::index_sequence<I...>) mutable {
                    ((array[I] = variable_name<(std::get<I>(tp))>()), ...);
                }(std::make_index_sequence<tuple_size>{});
#endif
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
                implements::get_member_names_compositor<type, tuple_size>(array);
#endif
                return array;
            }
#else
            static_assert(implements::always_false<Ty>,
                          "cannot get all the member names of Ty, please use marco RETTR_REFLECT_TUPLE_LIKE() to register");
#endif
        }
    }
}

namespace rettr {
    /**
     * @brief 将对象绑定为指针元组
     * @return 成员指针元组
     */
    template <typename Ty>
    constexpr auto struct_bind_tuple(Ty &obj) {
        constexpr std::size_t count = member_count_v<helper::remove_cvref_t<Ty>>;
        if constexpr (is_reflectet_for_type_valid<helper::remove_cvref_t<Ty>>) {
            return reflectet_for_type<Ty>::bind_obj(obj);
        } else if constexpr (count != 0 && std::is_aggregate_v<Ty>) {
            return implements::refl_to_tuple_impl<count, Ty>::make_ptr(obj);
        } else {
            return std::make_tuple();
        }
    }
}

namespace rettr::implements {
    template <typename Tuple>
    struct tuple_traits_impl {
        static inline constexpr bool invalid_mark = true;
    };

    template <typename... Types>
    struct tuple_traits_impl<std::tuple<Types...>> {
        using type = std::tuple<Types...>;

        template <std::size_t Idx>
        using element = std::tuple_element<Idx, type>;

        template <std::size_t Idx>
        using element_t = std::tuple_element_t<Idx, type>;

        static inline constexpr std::size_t size = sizeof...(Types);
    };

    template <typename Tuple>
    struct pair_traits_impl {
        static inline constexpr bool invalid_mark = true;
    };

    template <typename Ty1, typename Ty2>
    struct pair_traits_impl<std::pair<Ty1, Ty2>> {
        using type = std::pair<Ty1, Ty2>;
        using first_type = Ty1;
        using second_type = Ty2;
        static inline constexpr std::size_t size = 2;
    };
}

namespace rettr {
    template <typename Tuple>
    struct tuple_traits : implements::tuple_traits_impl<helper::remove_cvref_t<Tuple>> {};

    template <typename Tuple, typename = void>
    RETTR_CONSTEXPR_BOOL is_tuple_v = true;

    template <typename Tuple>
    RETTR_CONSTEXPR_BOOL is_tuple_v<Tuple, std::void_t<decltype(tuple_traits<Tuple>::invalid_mark)>> = false;

    template <typename Tuple>
    struct is_tuple : std::bool_constant<is_tuple_v<Tuple>> {};

    template <typename Pair>
    struct pair_traits : implements::pair_traits_impl<helper::remove_cvref_t<Pair>> {};

    template <typename Pair, typename = void>
    RETTR_CONSTEXPR_BOOL is_pair_v = true;

    template <typename Pair>
    RETTR_CONSTEXPR_BOOL is_pair_v<Pair, std::void_t<decltype(pair_traits<Pair>::invalid_mark)>> = false;

    template <typename Pair>
    struct is_pair : std::bool_constant<is_pair_v<Pair>> {};
}

namespace rettr::implements {
    template <typename Fx, typename Tuple, std::size_t... Is>
    constexpr decltype(auto) apply_impl(Fx &&fx, Tuple &&tuple, std::index_sequence<Is...>) {
        return std::forward<Fx>(fx)(
            std::forward<std::tuple_element_t<Is, std::decay_t<Tuple>>>(std::get<Is>(std::forward<Tuple>(tuple)))...);
    }

    template <typename Fx, typename Tuple>
    constexpr decltype(auto) apply_tuple(Fx &&fx, Tuple &&tuple) {
        constexpr std::size_t size = std::tuple_size<std::decay_t<Tuple>>::value;
        return apply_impl(std::forward<Fx>(fx), std::forward<Tuple>(tuple), std::make_index_sequence<size>{});
    }

    template <typename Obj, std::size_t... Is>
    constexpr auto make_pointer_tuple_impl(Obj &obj, std::index_sequence<Is...>) {
        return std::make_tuple(&std::get<Is>(obj)...);
    }

    template <typename Obj, std::size_t Count>
    constexpr auto make_pointer_tuple(Obj &obj) {
        return make_pointer_tuple_impl(obj, std::make_index_sequence<Count>{});
    }

    template <typename Ty, std::size_t... Is>
    constexpr auto make_array_pointer_tuple_impl(Ty &obj, std::index_sequence<Is...>) {
        return std::make_tuple(&obj[Is]...);
    }

    template <typename Ty, std::size_t count>
    constexpr auto make_array_pointer_tuple(Ty &obj) {
        return make_array_pointer_tuple_impl(obj, std::make_index_sequence<count>{});
    }

    template <typename ArrayType, std::size_t... Is>
    constexpr auto make_generic_array_pointer_tuple_impl(ArrayType &obj, std::index_sequence<Is...>) {
        return std::make_tuple(&obj[Is]...);
    }

    template <typename ArrayType, std::size_t count>
    constexpr auto make_generic_array_pointer_tuple(ArrayType &obj) {
        return make_generic_array_pointer_tuple_impl(obj, std::make_index_sequence<count>{});
    }

    template <typename Ty, std::size_t N, std::size_t... Is>
    constexpr auto make_std_array_pointer_tuple_impl(std::array<Ty, N> &obj, std::index_sequence<Is...>) {
        return std::make_tuple(&obj[Is]...);
    }

    template <typename Ty, std::size_t N>
    constexpr auto make_std_array_pointer_tuple(std::array<Ty, N> &obj) {
        return make_std_array_pointer_tuple_impl(obj, std::make_index_sequence<N>{});
    }
}

template <typename Ty1, typename Ty2>
struct rettr::reflectet_for_type<std::pair<Ty1, Ty2>> {
    static constexpr inline std::size_t count = 2;

    static constexpr auto make() noexcept {
        auto &pair = helper::get_fake_object<std::pair<Ty1, Ty2>>();
        return std::make_tuple(&pair.first, &pair.second);
    }

    static constexpr auto bind_obj(std::pair<Ty1, Ty2> &obj) noexcept {
        return std::make_tuple(&obj.first, &obj.second);
    }

    static constexpr auto member_names() noexcept {
        std::array<std::string_view, 2> names = {"first", "second"};
        return names;
    }
};

template <typename Tuple>
struct rettr::reflectet_for_type<Tuple, std::enable_if_t<rettr::is_tuple_v<Tuple>>> {
    static constexpr inline std::size_t count = tuple_traits<helper::remove_cvref_t<Tuple>>::size;

    static constexpr auto make() noexcept {
        auto &obj = helper::get_fake_object<helper::remove_cvref_t<Tuple>>();
        return implements::make_pointer_tuple<Tuple, count>(obj);
    }

    static constexpr auto bind_obj(Tuple &obj) noexcept {
        return implements::make_pointer_tuple<Tuple, count>(obj);
    }
};

template <typename Ty>
struct rettr::reflectet_for_type<Ty, std::enable_if_t<std::is_array_v<Ty>>> {
    static constexpr inline std::size_t count = helper::array_size_v<Ty>;

    static constexpr auto make() noexcept {
        auto &obj = helper::get_fake_object<Ty>();
        return implements::make_array_pointer_tuple(obj, count);
    }

    static constexpr auto bind_obj(Ty &obj) noexcept {
        return implements::make_array_pointer_tuple(obj, count);
    }

    static constexpr auto member_names() noexcept {
        std::array<std::string_view, 0> empty{};
        return empty;
    }
};

template <template <typename Ty, std::size_t N> typename ArrayTemplate, typename Ty, std::size_t N>
struct rettr::reflectet_for_type<ArrayTemplate<Ty, N>,
                          std::void_t<std::enable_if_t<std::is_aggregate_v<ArrayTemplate<Ty, N>>>,
                                      typename ArrayTemplate<Ty, N>::value_type, typename ArrayTemplate<Ty, N>::iterator>> {
    static constexpr inline std::size_t count = N;

    static constexpr auto make() noexcept {
        auto &obj = helper::get_fake_object<ArrayTemplate<Ty, N>>();
        return implements::make_generic_array_pointer_tuple(obj, count);
    }

    static constexpr auto bind_obj(ArrayTemplate<Ty, N> &obj) noexcept {
        return implements::make_generic_array_pointer_tuple(obj, count);
    }

    static constexpr auto member_names() noexcept {
        std::array<std::string_view, 0> empty{};
        return empty;
    }
};

template <template <typename Ty, std::size_t N> typename ArrayTemplate, typename Ty, std::size_t N>
struct rettr::reflectet_for_type<ArrayTemplate<Ty, N>, std::enable_if_t<std::is_same_v<ArrayTemplate<Ty, N>, std::array<Ty, N>>>> {
    static constexpr inline std::size_t count = N;

    static constexpr auto make() noexcept {
        if constexpr (count <= 256) {
            auto &obj = helper::get_fake_object<std::array<Ty, N>>();
            return implements::make_std_array_pointer_tuple<count>(obj);
        } else {
            return std::make_tuple();
        }
    }

    static constexpr auto bind_obj(ArrayTemplate<Ty, N> &obj) noexcept {
        if constexpr (count <= 256) {
            return implements::make_std_array_pointer_tuple<count>(obj);
        } else {
            return std::make_tuple();
        }
    }

    static constexpr auto member_names() noexcept {
        std::array<std::string_view, 0> empty{};
        return empty;
    }
};

#define RETTR_PTR_EACH(obj, member) &obj.member
#define RETTR_NAME_EACH(obj, member) #member

#define RETTR_REFLECT_TUPLE_LIKE(STRUCT, ...)                                                                                         \
    template <>                                                                                                                       \
    struct rettr::reflectet_for_type<STRUCT> {                                                                                        \
        static constexpr inline std::size_t count = RETTR_ARG_COUNT(__VA_ARGS__);                                                     \
                                                                                                                                      \
        static constexpr auto make() noexcept {                                                                                       \
            auto &obj = std::get_fake_object<STRUCT>();                                                                               \
            return std::make_tuple(RETTR_FE_FOR_EACH(RETTR_PTR_EACH, obj, __VA_ARGS__));                                              \
        }                                                                                                                             \
                                                                                                                                      \
        static constexpr auto bind_obj(STRUCT &obj) noexcept {                                                                        \
            return std::make_tuple(RETTR_FE_FOR_EACH(RETTR_PTR_EACH, obj, __VA_ARGS__));                                              \
        }                                                                                                                             \
                                                                                                                                      \
        static constexpr auto member_names() noexcept {                                                                               \
            std::array<std::string_view, count> names = {RETTR_FE_FOR_EACH(RETTR_NAME_EACH, dummy, __VA_ARGS__)};                     \
            return names;                                                                                                             \
        }                                                                                                                             \
    };

#define RETTR_REFLECT_TUPLE_LIKE_MARK_EMPTY(STRUCT)                                                                                   \
    template <>                                                                                                                       \
    struct rettr::reflectet_for_type<STRUCT> {                                                                                        \
        static constexpr inline std::size_t count = 0;                                                                                \
                                                                                                                                      \
        static constexpr auto make() noexcept {                                                                                       \
            auto &obj = std::get_fake_object<STRUCT>();                                                                               \
            return std::make_tuple();                                                                                                 \
        }                                                                                                                             \
                                                                                                                                      \
        static constexpr auto bind_obj(STRUCT &obj) noexcept {                                                                        \
            return std::make_tuple();                                                                                                 \
        }                                                                                                                             \
                                                                                                                                      \
        static constexpr auto member_names() noexcept {                                                                               \
            std::array<std::string_view, count> names = {};                                                                           \
            return names;                                                                                                             \
        }                                                                                                                             \
    };

#define RETTR_MEMPTR_EACH(TYPE, member) &TYPE::member
#define RETTR_PRIVATE_PTR_EACH(obj, ptr) (obj.*ptr)

#define RETTR_PRIVATE_REFLECT_TUPLE_LIKE(TYPE, ...)                                                                                   \
    namespace rettr::implements {                                                                                                     \
        template struct private_access<TYPE, RETTR_FE_FOR_EACH(RETTR_MEMPTR_EACH, TYPE, __VA_ARGS__)>;                                \
        inline constexpr auto get_private_ptrs(const private_access_tag_t<TYPE> &);                                                   \
    }                                                                                                                                 \
                                                                                                                                      \
    template <>                                                                                                                       \
    struct rettr::reflectet_for_type<TYPE> {                                                                                          \
        static constexpr inline std::size_t count = RETTR_ARG_COUNT(__VA_ARGS__);                                                     \
                                                                                                                                      \
        static constexpr auto make() noexcept {                                                                                       \
            using Ty = TYPE;                                                                                                          \
            auto &obj = std::get_fake_object<Ty>();                                                                                   \
            constexpr auto ptrs = implements::get_private_ptrs_helper<TYPE>::value;                                                   \
            return std::apply([&](auto... ptr) { return std::make_tuple(std::addressof(obj.*ptr)...); }, ptrs);                       \
        }                                                                                                                             \
                                                                                                                                      \
        static constexpr auto bind_obj(TYPE &obj) noexcept {                                                                          \
            using Ty = TYPE;                                                                                                          \
            constexpr auto ptrs = implements::get_private_ptrs_helper<TYPE>::value;                                                   \
            return std::apply([&](auto... ptr) { return std::make_tuple(std::addressof(obj.*ptr)...); }, ptrs);                       \
        }                                                                                                                             \
                                                                                                                                      \
        static constexpr auto member_names() noexcept {                                                                               \
            std::array<std::string_view, count> names = {RETTR_FE_FOR_EACH(RETTR_NAME_EACH, dummy, __VA_ARGS__)};                     \
            return names;                                                                                                             \
        }                                                                                                                             \
    };

namespace rettr::implements {
    template <typename Tuple, typename NamesArray, typename Fx, std::size_t... Idx>
    constexpr rettr_fn for_each_impl(Tuple &&tuple, NamesArray &&names, Fx &&func, std::index_sequence<Idx...>) -> void {
        (std::invoke(std::forward<Fx>(func), *std::get<Idx>(std::forward<Tuple>(tuple)),
                     std::get<Idx>(std::forward<NamesArray>(names)), Idx),
         ...);
    }

    template <typename Tuple, typename Fx, std::size_t... Idx>
    constexpr rettr_fn visit_members_impl(Tuple &&tuple, Fx &&func, std::index_sequence<Idx...>) -> void {
        std::invoke(std::forward<Fx>(func), *std::get<Idx>(std::forward<Tuple>(tuple))...);
    }
}

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::implements {
    template <typename Ty>
    inline constexpr auto member_offset_arr_cache = []() consteval {
        using type = helper::remove_cvref_t<Ty>;
        constexpr size_t count = member_count_v<type>;
        auto &fake = helper::get_fake_object<type>();
        auto tp = struct_to_tuple<type>();
        std::array<std::size_t, count> result{};
        [&]<std::size_t... Is>(std::index_sequence<Is...>) consteval {
            ((result[Is] = [&]() consteval -> std::size_t {
                 auto *ptr = std::get<Is>(tp);
                 template for (constexpr auto m: unchecked_all_member_array<Ty>) {
                     /*
                      * 自C++26开始，constexpr 的从 void* 转换 被允许，参见 P2738R1 提案
                      * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2738r1.pdf
                      */
                     if (static_cast<void *>(std::addressof(fake.[:m:])) == static_cast<void *>(ptr)) {
                         return std::meta::offset_of(m).bytes;
                     }
                 }
                 return (std::numeric_limits<std::size_t>::max)();
             }()),
             ...);
        }(std::make_index_sequence<count>{});
        return result;
    }();
}

#else

namespace rettr::implements {
    template <typename Ty, typename Tuple, size_t... Is>
    rettr_fn get_member_offset_arr_impl(Ty &t, Tuple &tp, std::index_sequence<Is...>) -> auto {
        std::array<std::size_t, sizeof...(Is)> arr;
        ((arr[Is] = static_cast<std::size_t>(reinterpret_cast<const char *>(std::get<Is>(tp)) - reinterpret_cast<char *>(&t))), ...);
        return arr;
    }
}

#endif

namespace rettr {
    template <typename Struct>
    constexpr rettr_fn tuple_to_variant() noexcept {
        using tuple_type_list = typename helper::tuple_like_to_type_list<decltype(struct_to_tuple<Struct>())>::type;
        using unique_type_list = helper::unique_type_list_t<tuple_type_list>;
        using variant_type = typename helper::type_list_to_tuple_like<unique_type_list, std::variant>::type;
        return variant_type{};
    }

    template <typename Ty>
    RETTR_STATIC_REFLECTION_CONSTEXPR26 rettr_fn get_member_offset_arr(Ty &&t) -> const std::array<std::size_t, member_count_v<Ty>> & {
        using type = helper::remove_cvref_t<Ty>;
        constexpr size_t count = member_count_v<type>;
#if RETTR_HAS_CXX26_STATIC_REFLECTION
        std::ignore = t;
        return implements::member_offset_arr_cache<Ty>;
#else
        auto tp = struct_bind_tuple(std::forward<Ty>(t));
        static std::array<size_t, count> arr = implements::get_member_offset_arr_impl(t, tp, std::make_index_sequence<count>{});
        return arr;
#endif
    }

    template <typename Ty>
    RETTR_STATIC_REFLECTION_CONSTEXPR26 RETTR_INLINE rettr_fn get_member_offset_arr()
        -> const std::array<std::size_t, member_count_v<Ty>> & {
        return get_member_offset_arr(helper::get_fake_object<Ty>());
    }

    template <typename Type, typename Fx>
    constexpr rettr_fn for_each(Fx &&func) -> void {
        if constexpr (std::is_invocable_v<Fx, std::string_view, std::size_t>) {
            auto names = get_member_names<Type>();
            for (std::size_t idx = 0; idx < names.size(); ++idx) {
                std::invoke(std::forward<Fx>(func), names[idx], idx);
            }
        } else if constexpr (std::is_invocable_v<Fx, std::string_view>) {
            for (const auto &item: get_member_names<Type>()) {
                std::invoke(std::forward<Fx>(func), item);
            }
        } else {
            static_assert(implements::always_false<Fx>, "Cannot accept this func");
        }
    }

    template <typename Object, typename Fx>
    constexpr rettr_fn for_each(Object &&object, Fx &&func) -> void {
        auto tuple = struct_bind_tuple<helper::remove_cvref_t<Object>>(object);
        implements::for_each_impl(tuple, get_member_names<Object>(), std::forward<Fx>(func),
                                  std::make_index_sequence<member_count_v<Object>>{});
    }

    template <typename Object, typename Visitor>
    constexpr rettr_fn visit_members(Object &&object, Visitor &&visitor) -> void {
        auto tuple = struct_bind_tuple<helper::remove_cvref_t<Object>>(object);
        implements::visit_members_impl(tuple, std::forward<Visitor>(visitor), std::make_index_sequence<member_count_v<Object>>{});
    }

    template <typename Ty, std::size_t Idx>
    constexpr rettr_fn name_of() noexcept -> std::string_view {
        auto names = get_member_names<Ty>();
        return std::get<Idx>(names);
    }

    template <typename Ty>
    constexpr rettr_fn name_of(std::size_t index) noexcept -> std::string_view {
        auto names = get_member_names<Ty>();
        if (index < names.size()) {
            return names[index];
        }
        return {};
    }

    template <typename Ty>
    constexpr rettr_fn index_of(std::string_view name) noexcept -> std::size_t {
        auto names = get_member_names<Ty>();
        std::size_t idx{0};
        for (const std::string_view &item: names) {
            if (name == item) {
                return idx;
            }
            ++idx;
        }
        return idx;
    }

#if RETTR_HAS_CXX20
    template <typename Ty, helper::basic_constexpr_string String>
    constexpr rettr_fn index_of() noexcept -> std::size_t {
        return index_of<Ty>({String.data(), String.length()});
    }
#endif

    template <std::size_t Idx, typename Ty>
    constexpr rettr_fn get(Ty &&object) noexcept -> decltype(auto) {
        return (*std::get<Idx>(struct_bind_tuple(object)));
    }

    template <typename Ty>
    constexpr rettr_fn get(Ty &&object, std::size_t idx) -> decltype(tuple_to_variant<helper::remove_cvref_t<Ty>>()) {
        auto tuple = struct_bind_tuple(object);
        auto ret = tuple_to_variant<helper::remove_cvref_t<Ty>>();
        constexpr auto find_fn = [](auto &&ret, const std::size_t index_, auto &&extract) {
            // NOLINT BEGIN
            std::apply(
                [&](auto &&...elems) { // NOLINT
                    std::size_t i{0};
                    (void) ((i++ == index_ ? ret = elems, true : false) || ...);
                },
                extract);
            // NOLINT END
        };
        find_fn(ret, idx, tuple);
        return ret;
    }

    template <typename Ty>
    constexpr rettr_fn get(Ty &&object, std::string_view name) -> decltype(tuple_to_variant<helper::remove_cvref_t<Ty>>()) {
        const std::size_t index = index_of<Ty>(name);
        return get<Ty>(std::forward<Ty>(object), index);
    }

#if RETTR_HAS_CXX20
    template <helper::basic_constexpr_string String, typename Ty>
    constexpr rettr_fn get(Ty &&object) noexcept -> decltype(auto) {
        constexpr std::size_t index = index_of<Ty, String>();
        return get<index>(std::forward<Ty>(object));
    }
#endif
}

namespace rettr {
    template <typename Ty>
    struct tuple_size : std::integral_constant<std::size_t, member_count_v<Ty>> {};

    template <typename Ty>
    static inline constexpr std::size_t tuple_size_v = tuple_size<Ty>::value;
}

#endif
