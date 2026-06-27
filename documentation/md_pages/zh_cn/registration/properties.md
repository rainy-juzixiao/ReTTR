# 属性注册

> 该章节介绍如何注册和访问属性（property），包括成员数据指针、getter/setter 函数以及 `operator()` 语法糖。

## 注册成员属性

### 直接绑定数据成员

最简单的形式是将名称与成员指针绑定：

```cpp
#include <rettr/registration>

struct Person {
    std::string name;
    int age = 0;

    RETTR_ENABLE()
};

RETTR_REGISTRATION {
    rettr::registration::class_<Person>("Person")
        .constructor<>()
        .property("name", &Person::name)
        .property("age", &Person::age);
}
```

### getter / setter 函数

当需要封装访问逻辑时，使用 getter / setter 函数对：

```cpp
struct Account {
    double get_balance() const { return balance_; }
    void set_balance(double val) { balance_ = val; }

private:
    double balance_ = 0.0;
    RETTR_REGISTRATION_FRIEND
};

RETTR_REGISTRATION {
    rettr::registration::class_<Account>("Account")
        .constructor<>()
        .property("balance", &Account::get_balance, &Account::set_balance);
}
```

### 只读属性

使用 `property_readonly` 注册仅有 getter 的属性：

```cpp
RETTR_REGISTRATION {
    rettr::registration::class_<Person>("Person")
        .property_readonly("name", &Person::name);  // 不可写
}
```

## 注册全局属性

非成员属性通过 `registration::property()` / `registration::property_readonly()` 注册：

```cpp
static const double Pi = 3.14159;
static std::string global_text;

const std::string& get_text() { return global_text; }
void set_text(const std::string& text) { global_text = text; }

RETTR_REGISTRATION {
    rettr::registration::property_readonly("Pi", &Pi);
    rettr::registration::property("global_text", &get_text, &set_text);
}
```

> [!NOTE]
> 全局属性名称不可重复。若注册同名属性，后注册者将被丢弃。

## 访问属性值

### 通过 type 快捷访问

使用 `type::property_value()` 读写：

```cpp
using namespace rettr;

Person obj;
type t = type::from<Person>();

// 写入
t.property_value("age", obj, 25);

// 读取
any val = t.property_value("age", obj);
std::cout << val.as<int>() << std::endl; // 25
```

全局属性忽略实例参数：

```cpp
any val = type::property_value("Pi");
std::cout << val.as<double>() << std::endl; // 3.14159
```

### 通过 property 对象访问

获取 property 句柄后反复使用，避免每次查找：

```cpp
property prop = t.property("age");

// 读取
any::reference ref = prop.value(obj);
std::cout << ref.as<int>() << std::endl;

// 写入
prop.value(obj, 30);
```

## operator() 语法糖

`property::operator()(object_view)` 返回 `any::reference` 代理对象，支持直接赋值和流输出（前提是该类型可被 `std::ostream` 输出）。

```cpp
property prop_name = t.property("name");
property prop_age  = t.property("age");

prop_age(obj) = 30;

std::cout << prop_age(obj) << std::endl;   // 30

prop_name(obj) = "Alice"; // 会自动构造std::string
std::cout << prop_name(obj) << std::endl;  // Alice
```

> [!NOTE]
> `operator()` 返回的是 `any::reference`，它是一个类型擦除的引用代理。赋值操作直接修改原始对象中的值，而非拷贝。

## 访问级别

注册时可指定访问级别，可与 C++ 访问控制对应：

```cpp
RETTR_REGISTRATION {
    rettr::registration::class_<Account>("Account")
        .property("balance", &Account::get_balance, &Account::set_balance,
                  rettr::registration::private_access)
        .property("name", &Account::name,
                  rettr::registration::protected_access)
        .property("age", &Account::age,
                  rettr::registration::public_access);
}
```

检索访问级别：

```cpp
property prop = t.property("balance");
if (prop.access_level() == access_levels::private_access) {
    std::cout << "私有属性" << std::endl;
}
```
