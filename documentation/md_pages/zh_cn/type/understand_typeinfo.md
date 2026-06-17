# 理解"typeinfo"类型

> 该章节将向你展示typeinfo类型的所有基本用法

## 概念

typeinfo类型是一个类型标识，类似于标准库的RTTI，但它只能对类型执行如下行为：

- 获取类型名称和hash_code
- 只能获取一些常见的is_xxx的特征检测
- 执行remove_xxx或decay的类型修改
- 获取template arguments，仅全类型模板可用
- 获取类型大小
- 获取

而面对较为复杂的操作，如需要RTTI的能力的时候，若需要执行dynamic_cast这种方法，typeinfo无法单独提供。需要和反射组件协作。

但无论如何，使用typeinfo的好处是，你不需要在运行时才能获取。严格来说，大部分能力均可以在编译时的上下文执行。并且，在某些时候，也比typeinfo更加灵活。

以及更重要的，你不需要专门使用type_index这样的wrapper，因为typeinfo是可以被拷贝的，也可以被移动。它是一个完整的值对象。

## 获取typeinfo

### 使用

使用typeinfo的最快入口就是使用rettr_typeid宏。rettr_typeid的用法与typeid非常接近，除了不能对对象使用。

### 输出结果

> [!NOTE]
> 结果仅供参考，其输出结果受平台或编译器影响

```c++
#include <rettr/typeinfo.hpp>

int main() {
    std::cout << rettr_typeid(int).name()        << '\n'; // int
    std::cout << rettr_typeid(char).name()       << '\n'; // char
    std::cout << rettr_typeid(int&).name()       << '\n'; // int&
    std::cout << rettr_typeid(std::string).name()    << '\n'; // [std::string](std::__cxx11::basic_string<char>)
    std::cout << rettr_typeid(std::nullptr_t).name() << '\n'; // std::nullptr_t
}
```

> [!NOTE]
> 下列的两个静态方法也能获取typeinfo对象，但并不推荐调用，除非有特殊需要。

- `typeinfo::create<T>()`: 可在编译时直接创建对象，但更推荐直接使用typeinfo对象
- `typeinfo::of<T>()`: 可在运行时获取静态单例对象，但rettr_typeid已进行包装，不建议直接使用，除非宏的调用产生了歧义，如包含了逗号

## 使用trait检查

ReTTR为typeinfo提供了以下固定trait的检查：

```c++
enum class traits {
    is_lref,
    is_rref,
    is_const,
    is_volatile,
    is_void,
    is_nullptr_t,
    is_integer,
    is_floating_point,
    is_unsigned,
    is_arithmetic,
    is_fundamental,
    is_trivial,
    is_pointer,
    is_array,
    is_class,
    is_union,
    is_enum,
    is_compound,
    is_function,
    is_function_pointer,
    is_member_fnptr,
    is_member_field_ptr,
    is_template,
    is_polymorphic,
    is_abstract,
    is_signed,
    is_sequential_container,
    is_associative_container
};
```

通常，提供了对一些方法对trait进行快捷检查。

- `is_arithmetic`
- `is_floating_point`
- `is_integer`
- `is_nullptr`
- `is_reference`
- `is_pointer`
- `is_lvalue_reference`
- `is_rvalue_reference`
- `is_const`
- `is_volatile`
- `is_const_volatile`
- `is_associative_container`
- `is_sequential_container`
- `is_class`

而未涉及的trait，则调用has_trait即可。

### 使用

下面这段代码展示了一部分对typeinfo的trait的使用。有一部分trait检查可以直接通过方法调用，其它的trait则需要has_traits ()方法

```c++
#include <rettr/typeinfo.hpp>

int main() {
    std::cout << std::boolalpha;
    std::cout << "Output Arithmetic trait: " << '\n';
    {
        std::cout << rettr_typeid(int).is_integer()                           << '\n'; // true
        std::cout << rettr_typeid(int).has_traits(rettr::traits::is_abstract) << '\n'; // false
        std::cout << rettr_typeid(int).has_traits(rettr::traits::is_trivial)  << '\n'; // true
        std::cout << rettr_typeid(char).is_integer()                          << '\n'; // true
        std::cout << rettr_typeid(char).is_floating_point()                   << '\n'; // false
        std::cout << rettr_typeid(char).is_arithmetic()                       << '\n'; // true
    }
    std::cout << "Output Reference trait: " << '\n';
    {
        std::cout << rettr_typeid(int).is_reference()           << '\n'; // false
        std::cout << rettr_typeid(int).is_lvalue_reference()    << '\n'; // false
        std::cout << rettr_typeid(int).is_rvalue_reference()    << '\n'; // false
        std::cout << rettr_typeid(int &).is_reference()         << '\n'; // true
        std::cout << rettr_typeid(int &&).is_reference()        << '\n'; // true
        std::cout << rettr_typeid(int &).is_lvalue_reference()  << '\n'; // true
        std::cout << rettr_typeid(int &&).is_lvalue_reference() << '\n'; // false
        std::cout << rettr_typeid(int &).is_rvalue_reference()  << '\n'; // false
        std::cout << rettr_typeid(int &&).is_rvalue_reference() << '\n'; // true
    }
    std::cout << "Output Class trait: " << '\n';
    {
        using unorder_map = std::unordered_map<int, char>; // 由于

        std::cout << rettr_typeid(std::string).is_class()                      << '\n'; // true
        std::cout << rettr_typeid(std::vector<int>).is_sequential_container()  << '\n'; // true
        std::cout << rettr_typeid(std::vector<int>).is_associative_container() << '\n'; // false
        std::cout << rettr_typeid(unorder_map).is_sequential_container()       << '\n'; // false
        std::cout << rettr_typeid(unorder_map).is_associative_container()      << '\n'; // true
    }
    std::cout << "Const/Voltaile trait: " << '\n';
    {
        std::cout << rettr_typeid(int).is_const()                   << '\n'; // false
        std::cout << rettr_typeid(int).is_volatile()                << '\n'; // false
        std::cout << rettr_typeid(const int).is_const()             << '\n'; // true
        std::cout << rettr_typeid(const int).is_volatile()          << '\n'; // false
        std::cout << rettr_typeid(const volatile int).is_const()    << '\n'; // true
        std::cout << rettr_typeid(const volatile int).is_volatile() << '\n'; // true
    }
}
```

> [!TIP]
> has_traits ()可以执行组合查询，例如下列代码代表检查代码是否同时具备is_pointer和is_const的特性
> has_traits (rettr::traits::is_pointer | rettr::traits::is_const)

### 输出结果

```text
Output Arithmetic trait: 
true
false
true
true
false
true
Output Reference trait: 
false
false
false
true
true
true
false
false
true
Output Class trait: 
true
true
false
false
true
Const/Voltaile trait: 
false
false
true
false
true
true
```

## 使用类型修改

typeinfo提供了如下方法进行类型修改

- `remove_const`
- `remove_volatile`
- `remove_reference`
- `remove_cvref`
- `remove_pointer`
- `decay`

### 使用

```c++
#include <rettr/typeinfo.hpp>

int main() {
    {
        std::cout << rettr_typeid(int).remove_const().name() << '\n'; // int
    }
    {
        std::cout << rettr_typeid(const int).remove_const().name()        << '\n'; // int
        std::cout << rettr_typeid(volatile int).remove_volatile().name()  << '\n'; // int
    }
    {
        std::cout << rettr_typeid(const std::string &).remove_reference().name()    << '\n'; // const std::__cxx11::basic_string<char>
        std::cout << rettr_typeid(const std::string &).remove_cvref().name()        << '\n'; // std::__cxx11::basic_string<char>
    }
    {
        std::cout << rettr_typeid(const std::string[10]).decay().name()                 << '\n'; // const std::__cxx11::basic_string<char>*
        std::cout << rettr_typeid(const char *).remove_pointer().remove_const().name()  << '\n'; // char
    }
}
```

> [!TIP]
> 所有类型修改操作都会产生一个新的typeinfo对象，你可以继续调用或检查。不会影响原对象的使用。

### 输出结果

```text
int
int
int
const std::__cxx11::basic_string<char>
std::__cxx11::basic_string<char>
const std::__cxx11::basic_string<char>*
char
```

## 比较类型

### is_same/operator==

若要比较是否相等，可以直接使用is_same方法或是typeinfo为其重载的operator==

```c++
std::cout << rettr_typeid(int).is_same(rettr_typeid(const int))  << '\n'; // true
std::cout << rettr_typeid(int).is_same(rettr_typeid(int&))       << '\n'; // false

std::cout << rettr_typeid(int) == rettr_typeid(int)   << '\n'; // true
std::cout << rettr_typeid(int) == rettr_typeid(int&)  << '\n'; // false
```

### is_compatbile

is_compatible相比is_same更加特殊，它主要接收一个可能要被转换到的同构类型。它按照如下规则执行

> [!TIP]
> 同构类型在此处是指带有各种修饰符的同一类型，如int*、int&、const int*等都是int的同构类型

1. 对于特殊类型，进行如下处理:

| 类型           | 行为                                       |
|----------------|--------------------------------------------|
| void           | 直接返回 false                             |
| std::nullptr_t | 仅当目标类型为 std::nullptr_t 时返回 true  |
| 函数类型       | 仅允许精确匹配，不支持 cv/引用限定符的转换 |

2. 常规类型 对原始类型 T（移除 cv 和引用后的类型）进行匹配：

| 目标类型                                                               | 匹配策略                                       |
|------------------------------------------------------------------------|------------------------------------------------|
| `T、const T、const volatile T`                                         | 精确匹配，直接返回 true                        |
| `T&、T&&、const T&、const T&&、const volatile T&、const volatile T&& ` | 检查对应引用类型能否隐式转换为 Type 的引用形式 |

```c++
#include <rettr/typeinfo.hpp>

int main() {
    std::cout << std::boolalpha;
    std::cout << "Check no-cv type: \n";
    {
        std::cout << rettr_typeid(int).is_compatible(rettr_typeid(void)) << '\n';
        std::cout << rettr_typeid(int).is_compatible(rettr_typeid(const int)) << '\n';
        std::cout << rettr_typeid(int).is_compatible(rettr_typeid(int)) << '\n';
        std::cout << rettr_typeid(int).is_compatible(rettr_typeid(volatile int)) << '\n';
        std::cout << rettr_typeid(int).is_compatible(rettr_typeid(const char)) << '\n';
    }
    std::cout << "Check lref type: \n";
    {
        std::cout << rettr_typeid(int&).is_compatible(rettr_typeid(void)) << '\n';
        std::cout << rettr_typeid(int&).is_compatible(rettr_typeid(const int)) << '\n';
        std::cout << rettr_typeid(int&).is_compatible(rettr_typeid(int)) << '\n';
        std::cout << rettr_typeid(int&).is_compatible(rettr_typeid(volatile int)) << '\n';
        std::cout << rettr_typeid(int&).is_compatible(rettr_typeid(const int&)) << '\n';
    }
    std::cout << "Check rref type: \n";
    {
        std::cout << rettr_typeid(int&&).is_compatible(rettr_typeid(void)) << '\n';
        std::cout << rettr_typeid(int&&).is_compatible(rettr_typeid(const int)) << '\n';
        std::cout << rettr_typeid(int&&).is_compatible(rettr_typeid(int)) << '\n';
        std::cout << rettr_typeid(int&&).is_compatible(rettr_typeid(volatile int)) << '\n';
        std::cout << rettr_typeid(int&&).is_compatible(rettr_typeid(const int&)) << '\n';
    }
}
```

```text
Check no-cv type: 
false
true
true
false
false
Check lref type: 
false
true
true
false
false
Check rref type: 
false
true
true
false
false
false
```

## 获取模板参数

### 使用

```c++
#include <rettr/typeinfo.hpp>

int main() {
    std::cout << "Check std::vector<int>: \n";
    {
        auto arguments = rettr_typeid(std::vector<int>).template_arguments();
        for (const auto& i : arguments) {
            std::cout << i.name() << '\n';
        }
    }
    std::cout << "Check std::unordered_map<int, std::string>: \n";
    {
        using t = std::unordered_map<int, std::string>;
        auto arguments = rettr_typeid(t).template_arguments();
        for (const auto& i : arguments) {
            std::cout << i.name() << '\n';
        }
    }
    return 0;
}
```

### 输出

```text
Check std::vector<int>: 
int
std::allocator<int>
Check std::unordered_map<int, std::string>: 
int
std::__cxx11::basic_string<char>
std::hash<int>
std::equal_to<int>
std::allocator<std::pair<const int, std::__cxx11::basic_string<char> > >
```