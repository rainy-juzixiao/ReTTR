#ifndef RETTR_CORE_TEMPLATE_TRAITS_HPP
#define RETTR_CORE_TEMPLATE_TRAITS_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/core/type_list.hpp>

namespace rettr {
    /**
     * @brief Primary template for template traits.
     *        Provides information about whether a type is a template instantiation.
     *
     *        模板特性的主模板。
     *        提供关于类型是否为模板实例化的信息。
     *
     * @tparam Template The type to examine
     *                  要检查的类型
     */
    template<typename Template>
    struct template_traits : std::false_type {
    };

    /**
     * @brief Specialization for actual template instantiations.
     *        Extracts the template template parameter and the type arguments.
     *
     *        实际模板实例化的特化。
     *        提取模板模板参数和类型参数。
     *
     * @tparam Template The template template parameter
     *                  模板模板参数
     * @tparam Types The template arguments
     *               模板参数
     */
    template<template <typename...> typename Template, typename... Types>
    struct template_traits<Template<Types...> > : std::true_type {
        /**
         * @brief Type list containing all template arguments.
         *        包含所有模板参数的类型列表。
         */
        using types = helper::type_list<Types...>;
    };

    /**
     * @brief Variable template for checking if a type is a template instantiation.
     *        检查类型是否为模板实例化的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL is_template_v = template_traits<Ty>::value;

    /**
     * @brief Type template for checking if a type is a template instantiation.
     *        检查类型是否为模板实例化的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct is_template : std::bool_constant<is_template_v<Ty> > {
    };

    /**
 * @brief Variable template for checking if a type is a specialization of a template.
 *        Detects whether Type is an instantiation of Template.
 *
 *        检索类型是否具有特化的变量模板。
 *        检测 Type 是否为 Template 的实例化。
 *
 * @tparam Type The type to check
 *              要检查的类型
 * @tparam Template The template to check against
 *                  要检查的特化模板
 */
    template<typename Type, template <typename...> typename Template>
    RETTR_CONSTEXPR_BOOL is_specialization_v = false;

    template<template <typename...> typename Template, typename... Types>
    RETTR_CONSTEXPR_BOOL is_specialization_v<Template<Types...>, Template> = true;

    /**
     * @brief Type template for checking if a type is a specialization of a template.
     *        Inherits from bool_constant based on is_specialization_v.
     *
     *        检索类型是否具有特化的类型模板。
     *        基于 is_specialization_v 继承自 bool_constant。
     *
     * @tparam Type The type to check
     *              要检查的类型
     * @tparam Template The template to check against
     *                  要检查的特化模板
     */
    template<typename Type, template <typename...> typename Template>
    struct is_specialization : std::bool_constant<is_specialization_v<Type, Template> > {
    };
}

#endif
