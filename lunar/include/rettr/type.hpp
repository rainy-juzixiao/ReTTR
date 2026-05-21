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
#ifndef RETTR_TYPE_HPP
#define RETTR_TYPE_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/string_view.hpp>
#include <rettr/array_range.hpp>
#include <rettr/filter_item.hpp>

namespace rettr {
    class variant;
    class constructor;
    class destructor;
    class method;
    class property;
    class enumeration;
    class type;
    class instance;
    class argument;
    class visitor;
}

namespace rettr::implements {
    struct derived_info;
    struct base_class_info;
    struct type_converter_base;
    class type_register;
    class type_register_private;

    static type invalid_type() noexcept;

    struct invalid_type {
    };

    struct type_data;
    struct class_data;
    class destructor_wrapper_base;
    class property_wrapper_base;
    RETTR_LOCAL_API RETTR_INLINE type create_type(type_data *) noexcept;

    template<typename T>
    RETTR_LOCAL_API std::unique_ptr<type_data> make_type_data();

    template<typename T, typename Tp, typename Converter>
    struct variant_data_base_policy;

    struct type_comparator_base;

    enum class type_of_visit : bool;

    RETTR_API bool compare_types_less_than(const void *, const void *, const type &, int &);

    RETTR_API bool compare_types_equal(const void *, const void *, const type &, bool &);

    template<typename T>
    RETTR_LOCAL_API RETTR_INLINE type type_from_instance(const T *) noexcept;
}

namespace rettr {
    template<typename Target_Type, typename Source_Type>
    Target_Type rettr_cast(Source_Type object) noexcept;

    class RETTR_API type {
    public:
        using type_id = uintptr_t;

        RETTR_INLINE type(const type &other) noexcept;

        RETTR_INLINE type &operator=(const type &other) noexcept;

        RETTR_INLINE bool operator<(const type &other) const noexcept;

        RETTR_INLINE bool operator>(const type &other) const noexcept;

        RETTR_INLINE bool operator>=(const type &other) const noexcept;

        RETTR_INLINE bool operator<=(const type &other) const noexcept;

        RETTR_INLINE bool operator==(const type &other) const noexcept;

        RETTR_INLINE bool operator!=(const type &other) const noexcept;

        RETTR_INLINE type_id id() const noexcept;

        RETTR_INLINE string_view name() const noexcept;

        RETTR_INLINE bool empty() const noexcept;

        RETTR_INLINE explicit operator bool() const noexcept;

        RETTR_INLINE type raw_type() const noexcept;

        RETTR_INLINE type wrapped_type() const noexcept;

        template<typename T>
        RETTR_LOCAL_API static type from() noexcept;

        template<typename T>
        RETTR_LOCAL_API static type from(T &&object) noexcept;

        static type from_name(string_view name) noexcept;

        static array_range<type> types() noexcept;

        RETTR_INLINE std::size_t size_of() const noexcept;

        RETTR_INLINE bool is_class() const noexcept;

        RETTR_INLINE bool is_template_instantiation() const noexcept;

        array_range<type> template_arguments() const noexcept;

        RETTR_INLINE bool is_enumeration() const noexcept;

        enumeration enumeration() const noexcept;

        RETTR_INLINE bool is_wrapper() const noexcept;

        RETTR_INLINE bool is_array() const noexcept;

        RETTR_INLINE bool is_associative_container() const noexcept;

        RETTR_INLINE bool is_sequential_container() const noexcept;

        RETTR_INLINE bool is_pointer() const noexcept;

        RETTR_INLINE bool is_arithmetic() const noexcept;

        RETTR_INLINE bool is_function_pointer() const noexcept;

        RETTR_INLINE bool is_member_object_pointer() const noexcept;

        RETTR_INLINE bool is_member_function_pointer() const noexcept;

        bool is_derived_from(const type &other) const noexcept;

        template<typename T>
        bool is_derived_from() const noexcept;

        bool is_base_of(const type &other) const noexcept;

        template<typename T>
        bool is_base_of() const noexcept;

        array_range<type> base_classes() const noexcept;

        array_range<type> derived_classes() const noexcept;

        variant metadata(const variant &key) const;

        constructor constructor(const std::vector<type> &params = std::vector<type>()) const noexcept;

        array_range<rettr::constructor> constructors() const noexcept;

        array_range<rettr::constructor> constructors(filter_items filter) const noexcept;

        variant create(std::vector<argument> args) const;

        destructor destructor() const noexcept;

        bool destroy(variant &obj) const noexcept;

        property property(string_view name) const noexcept;

        array_range<rettr::property> properties() const noexcept;

        array_range<rettr::property> properties(filter_items filter) const noexcept;

        static rettr::property global_property(string_view name) noexcept;

        static array_range<rettr::property> global_properties() noexcept;

        variant property_value(string_view name, instance obj) const;

        static variant property_value(string_view name);

        bool set_property_value(string_view name, instance obj, argument arg) const;

        static bool set_property_value(string_view name, argument arg);

        method method(string_view name) const noexcept;

        rettr::method method(string_view name, const std::vector<type> &type_list) const noexcept;

        array_range<rettr::method> methods() const noexcept;

        array_range<rettr::method> methods(filter_items filter) const noexcept;

        static rettr::method global_method(string_view name) noexcept;

        static rettr::method global_method(string_view name, const std::vector<type> &params) noexcept;

        static array_range<rettr::method> global_methods() noexcept;

        variant invoke(string_view name, instance obj, std::vector<argument> args) const;

        static variant invoke(string_view name, std::vector<argument> args);

        template<typename F>
        static void register_converter_func(F func);

        template<typename T>
        static void register_wrapper_converter_for_base_classes();

        template<typename T>
        static void register_comparators();

        template<typename T>
        static void register_equal_comparator();

        template<typename T>
        static void register_less_than_comparator();

    private:
        type() noexcept;

        RETTR_INLINE explicit type(implements::type_data *data) noexcept;

        static void *apply_offset(void *ptr, const type &source_type, const type &target_type) noexcept;

        static type derived_type(void *ptr, const type &source_type) noexcept;

        const implements::type_converter_base *type_converter(const type &target_type) const noexcept;

        const implements::type_comparator_base *equal_comparator() const noexcept;

        const implements::type_comparator_base *less_than_comparator() const noexcept;

        RETTR_INLINE std::size_t pointer_dimension() const noexcept;

        RETTR_INLINE type raw_array_type() const noexcept;

        RETTR_INLINE string_view full_name() const noexcept;

        void create_wrapped_value(const argument &arg, variant &var) const;

        void visit(visitor &visitor, implements::type_of_visit visit_type) const noexcept;

        RETTR_INLINE variant create_variant(const argument &data) const;

        friend class variant;

        template<typename Target_Type, typename Source_Type>
        friend Target_Type rettr_cast(Source_Type object) noexcept;

        friend class instance;
        friend class implements::type_register;
        friend class implements::type_register_private;
        friend class visitor;
        friend struct implements::class_data;

        friend type implements::create_type(implements::type_data *) noexcept;

        template<typename T>
        friend std::unique_ptr<implements::type_data> implements::make_type_data();

        template<typename T, typename Tp, typename Converter>
        friend struct implements::variant_data_base_policy;

        friend RETTR_API bool implements::compare_types_less_than(const void *, const void *, const type &, int &);

        friend RETTR_API bool implements::compare_types_equal(const void *, const void *, const type &, bool &);

    private:
        implements::type_data *type_data_;
    };
}

//#include <rettr/implements/type/type_impl.h> // ... detail

// ============================================================================
// TODO: 类型系统基础设施
// ============================================================================
// TODO: 完整定义 type_data 结构体
//       - 包含: type_id, name, size, 类型特征标志位等
// TODO: 实现全局类型注册表 (type_register)
//       - 提供线程安全支持
// TODO: 实现类型 ID 生成机制
//       - 基于类型哈希或全局计数器

// ============================================================================
// TODO: 继承层次支持
// ============================================================================
// TODO: 完善 base_class_info 与 derived_info
//       - 存储偏移量与类型转换信息
// TODO: 实现多继承/虚基类偏移量计算
// TODO: 实现指针调整逻辑
// TODO: 实现 RETTR_ENABLE 宏
//       - 用于注册类层次结构

// ============================================================================
// TODO: 包装器类型系统
// ============================================================================
// TODO: 实现 wrapper_mapper 特化机制
//       - 支持 std::shared_ptr / std::unique_ptr 等包装器识别
// TODO: 实现 create_wrapped_value
//       - 支持包装器值创建与提取
// TODO: 实现包装器类型转换注册
//       - register_wrapper_converter_for_base_classes

// ============================================================================
// TODO: 容器支持
// ============================================================================
// TODO: 实现 sequential_container_mapper
// TODO: 实现 associative_container_mapper
// TODO: 实现容器迭代器适配
// TODO: 实现元素访问接口
// TODO: 实现容器大小/容量查询接口

// ============================================================================
// TODO: 类型转换系统
// ============================================================================
// TODO: 实现 type_converter_base 虚基类
// TODO: 实现类型转换函数注册机制
// TODO: 支持隐式转换查找与调用
// TODO: 支持用户定义转换查找与调用
// TODO: 实现 apply_offset
//       - 用于派生类到基类转换偏移应用

// ============================================================================
// TODO: 比较器系统
// ============================================================================
// TODO: 实现 type_comparator_base 虚基类
// TODO: 实现相等比较器注册与调用
// TODO: 实现小于比较器注册与调用
// TODO: 实现复合类型递归比较
//       - 如 tuple 等结构

// ============================================================================
// TODO: 元数据系统
// ============================================================================
// TODO: 实现键值对元数据存储
//       - 基于 variant 作为 key/value
// TODO: 实现元数据查询接口
// TODO: 实现元数据遍历接口

// ============================================================================
// TODO: 构造/析构系统
// ============================================================================
// TODO: 实现 constructor 包装器
// TODO: 实现 destructor 包装器
// TODO: 支持默认参数处理
// TODO: 实现对象创建/销毁内存管理

// ============================================================================
// TODO: 属性系统
// ============================================================================
// TODO: 实现 property_wrapper_base
// TODO: 实现成员指针适配
// TODO: 实现 getter/setter 封装
// TODO: 区分静态属性与实例属性

// ============================================================================
// TODO: 方法系统
// ============================================================================
// TODO: 实现 method 包装器
// TODO: 实现参数类型匹配
// TODO: 实现成员函数指针适配
// TODO: 实现重载函数分辨与调用

// ============================================================================
// TODO: 变体类型系统
// ============================================================================
// TODO: 实现 variant_data_base_policy 策略类
// TODO: 实现类型擦除机制
// TODO: 实现类型安全存储
// TODO: 支持值语义与移动语义

// ============================================================================
// TODO: 访问者模式
// ============================================================================
// TODO: 实现 visitor 基类
// TODO: 实现类型遍历机制
// TODO: 定义 type_of_visit 枚举

#endif
