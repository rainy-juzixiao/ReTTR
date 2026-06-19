## rettr_cast vs. dynamic_cast

rettr_cast 函数的行为类似于标准的 C++ dynamic_cast。但是，它有一些优势：

- 它不需要 RTTI
- 支持，可以跨动态库边界工作，并且速度更快。

少有的不足是，它因为在C++26之前没有动态反射导致你必须使用`RETTR_ENABLE()`来提示ReTTR的类型系统。

对于性能，可通过运行基准测试目标`bench_rettr_cast`以在本地计算机上执行此测试。

## rettr::rettr_cast<T>()

rettr_cast 函数允许客户端在类层次结构之间进行向上和向下转换，在不相关的类之间进行交叉转换，甚至可以在具有虚继承的类层次结构中进行转换。目标类型
T 也可以位于层次结构的中间。

```c++
struct A { RETTR_ENABLE () };

struct B : A { RETTR_ENABLE (A) };

struct C : B { RETTR_ENABLE (B) };

C c;

A* a = &c;

B* b = rettr_cast<B*>(a); // 成功
```

当然，也可以进行交叉类型转换：

```c++
struct A { RETTR_ENABLE () };

struct B { RETTR_ENABLE () };

struct C : A , B { RETTR_ENABLE (A, B) };

C c;

A* a = &c;

B* b = rettr_cast<B*>(a); // 成功；注意类 A 和 B 彼此无关
```

类型对象知道它派生自哪个父类。假设此信息通过 RETTR_ENABLE 提供。

> [!IMPORTANT]
> 目前实现，其目标类型 T 只能是指针类型。

## 总结

- 当需要对给定实例执行向下类型转换或交叉类型转换时，请使用 rettr_cast。