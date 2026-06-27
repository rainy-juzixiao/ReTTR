# 从rettr::type获取类型信息

> 本节展示了如何使用rettr::type获取类型信息。

在你了解了rettr::typeinfo之后，有一点可以说明的是，rettr::type的情况会特殊一些。它是基于typeinfo封装的，在运行时的代码，所以你不能使用constexpr进行编译期计算。

而rettr::type它并非纯粹的值对象，而是一个中间人句柄。来自一个中心注册表，但在此，让我们结束这些实现细节，直白说吧。
在ReTTR中，因为rettr::type基于rettr::typeinfo封装，这意味着你可以在目标应用程序中完全禁用 RTTI，而改用 ReTTR。

既然，已经说过了rettr::type是一个中间人句柄。那么，所有类型相关操作都要通过这个中间人进行，它就是检索类型对象或查询所有类型信息的核心类。

那么，直接开门见山，首先rettr::type可以被默认进行空构造，可以执行拷贝、移动的构造和赋值修改。但是不能传入其它参数进行构造，你只能使用它提供的static工厂函数获取type。

最快的获取方式是使用rettr::from<T>()进行获取。下面的代码展示了最快的获取方式。

```c++
#include <rettr/type> // 必须添加

using namespace rettr; // 可加可不加，请按照团队规范决定

type my_int_type  = type::from<int>();     // 直接静态获取

type my_bool_type = type::from(true);      // 根据表达式的最终类型获取
```

## rettr::type::from<T>()

该函数仅要求一个模板类型参数，很适合用于在你能获取某个类型的情况下的时候使用。

下面的代码演示了如何获取并将其用于比较信息。

```c++
    type::from<int>() == type::from<int>();     // true

    type::from<int>() == type::from<bool>();    // false
```

> [!TIP]
> `from<T>`可以被认为是从`T`这个类型中取出中间人的类型信息。

另注：在比较类型时，内部总是进行简单的hash_code比较，而不是直接比较字符串。

> [!CAUTION]
> 不要传入typeinfo类型作为类型参数给此函数，否则会导致编译错误。

## rettr::type::from<T>(T&& obj)

此函数接受一个通用引用，并从每个给定的对象返回相应的 rettr::type 对象。

```c++
    int int_obj = 23;
    int* int_obj_ptr         = &int_obj;
    const int* c_int_obj_ptr = int_obj_ptr;
  
    type::get<int>()        == type::get(int_obj);        // true
    type::get<int*>()       == type::get(int_obj_ptr);    // true
    type::get<const int*>() == type::get(c_int_obj_ptr);  // true
```

当对类型为多态类类型的 [gvalue](https://zh.cppreference.com/cpp/language/value_category#%E6%B3%9B%E5%B7%A6%E5%80%BC)
表达式调用此函数时，且当前所属类已启用`RETTR_ENABLE()`宏的时候，其最终结果引用一个 rettr::type 对象，表示最派生对象的类型。

> [!WARNING]
> 下列代码假设你已经使用了`RETTR_ENABLE()`宏，若你没有使用，否则代码全部无效

```c++
    struct Base {};
    struct Derived : Base {};
    Derived d;
    Base& base = d;

    type::from<Derived>()  == type::from(base)        // true
    type::from<Base>()     == type::from(base)        // false

    // REMARK when called with pointers:
    Base* base_ptr = &d;
    type::from<Derived>()  == type::from(base_ptr);   // false
    type::from<Base*>()    == type::from(base_ptr);   // true
```

如果表达式的类型是 cv 限定类型，则 rettr::type::from() 表达式的结果是引用一个非cv限定的类型。但 `rettr::type::from<T>` 不适用于该项规则。参见下列代码。

```c++
class D { ... };
D d1;
const D d2;
type::from(d1)  == type::from(d2);         // true
type::from<D>() == type::from<const D>();  // false
type::from<D>() == type::from(d2);         // true
type::from<D>() == type::from<const D&>(); // false
type::from<D>() == type::from<const D*>(); // false
```

> [!CAUTION]
> 不要传入typeinfo类的对象给此函数，否则会导致编译错误。

## rettr::type::from_name (string_view)

此函数仅接受类型名称作为参数。当您只知道类型名称而无法将类型本身包含在源代码中时，此函数非常有用。

```c++
type::from_name("int")  == type::from<int>();   // true
type::from_name("bool") == type::from<int>();  // false
type::from_name("MyNameSpace::MyStruct") == type::from<MyNameSpace::MyStruct>();  // true，此处假定MyNameSpace::MyStruct已经注册
```

> [!IMPORTANT]
> 在使用type::from_name()之前，必须先调用 `type::from<T>()` 使用该函数一次。否则，该类型将不会在类型系统中注册，若要注册类类型，请参照注册章节执行。

## rettr::from_typeid()

此函数仅接受typeinfo类型标识作为参数。当您只知道有类型标识，而无法将类型本身包含在源代码中时，此函数非常有用。

> [!IMPORTANT]
> 在使用type::from_typeid()之前，必须先调用 `type::from<T>()` 使用该函数一次。否则，该类型将不会在类型系统中注册，若要注册类类型，请参照注册章节执行。

```c++
type::from_typeid(rettr_typeid(int))  == type::from<int>();   // true
type::from_typeid(rettr_typeid(bool)) == type::from<int>();  // false
type::from_typeid(rettr_typeid(MyNameSpace::MyStruct)) == type::from<MyNameSpace::MyStruct>();  // true，此处假定MyNameSpace::MyStruct已经注册
```

## rettr::type::from_base()

此函数用于从基类指针获取派生类的类型信息。当您拥有一个指向基类的指针，但实际对象是派生类实例，且需要获取其真实类型时，该函数非常有用。

```c++
struct Base {};
struct Derived : Base {};

Derived d;
Base* base_ptr = &d;

type derived_type = type::from_base(base_ptr);
type::from<Derived>() == derived_type;  // true，因为实际对象是 Derived

// 可以为一个指针指定一个基类类型
type source_type = type::from<Base>();
type derived_type2 = type::from_base(static_cast<void*>(base_ptr), source_type);
type::from<Derived>() == derived_type2;  // true
```

> [!IMPORTANT]
> 使用 from_base() 的前提是，目标类（派生类）已通过 RETTR_ENABLE() 宏启用注册，否则函数无法获取派生类型信息。

> [!TIP]
> 当您通过基类指针操作对象，但在运行时需要获知其真实派生类型以执行特定逻辑。从外部模块或动态库中获得了一个基类指针，而派生类型定义不在当前编译单元中，但已注册到 ReTTR 类型系统。

## 将rettr::type用于自定义容器存储

通用，因为rettr::type可以允许复制、移动。因此，你也可以将rettr::type应用到容器存储中。

```c++
std::vector<rettr::type> type_list;
std::map<rettr::type, std::string> mapping;
std::unordered_map<rettr::type, std::string> type_names;
```

## 另注

若你使用过RTTR库，请注意，ReTTR的API在此处与RTTR存在出入，如from_base是基于rttr的type类的private修饰的get_derived_type函数改进的。
并且为了实现与typeinfo的互通，因此加入了from_typeid并禁止了typeinfo类被传入from函数中。整体上，ReTTR的获取API更接近于标准库的风格。

## 总结

- rettr::type 是基于 rettr::typeinfo 封装的运行时类型句柄，不能在编译期使用 constexpr 计算，且允许在完全禁用 RTTI 的环境下使用。
- 它是类型操作的核心中间人，所有类型信息检索都通过它进行；支持默认构造、拷贝和移动，但只能通过静态工厂方法创建实例。
- 主要获取方式包括：
  - type::from<T>()：直接通过模板类型参数获取类型句柄。 
  - type::from<T>(T&& obj)：根据对象表达式获取类型，支持多态派生类型识别（需启用 `RETTR_ENABLE()`），且会忽略 cv 限定符。 
  - type::from_name(string_view)：通过类型名称字符串获取，但需确保该类型已预先注册。 
  - type::from_typeid(typeinfo)：通过 typeid 标识获取，同样需预先注册。
  - type::from_base()：通过基类指针获取派生类的真实类型信息，支持 void* 手动指定源类型和模板自动推导两种方式，需要派生类已注册。
- 类型比较基于内部哈希码，而非字符串比较。
- rettr::type 可复制、移动，因此能方便地存入 vector、map、unordered_map 等标准容器中。
- 对于RTTR用户来说，ReTTR用于获取type的接口更接近于标准库的风格而不是传统的OOP风格，请在此注意。