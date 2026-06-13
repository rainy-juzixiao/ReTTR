#include <iostream>
#include <random>
#include <rettr/registration.hpp>
#include <rettr/rettr_cast.hpp>
#include <rettr/rettr_enable.hpp>

#include <rettr/object.hpp>

class MyClass {
public:
    MyClass() = default;

    MyClass(int a, int b, bool add_one = false) {
    }


    void hello() {
        std::cout << "Hello" << '\n';
    };

    int add(int a, int b, bool add_one) {
        return a + b;
    }

    int field{10};
};

class Base {
    ENABLE_RETTR_CAST()
public:
    virtual ~Base() {
    }
};

class Level1 : public Base {
    ENABLE_RETTR_CAST(Base)
public:
    virtual ~Level1() {
    }
};

class Level2 : public Level1 {
    ENABLE_RETTR_CAST(Level1)
public:
    virtual ~Level2() {
    }
};

class Level3 : public Level2 {
    ENABLE_RETTR_CAST(Level2)
public:
    virtual ~Level3() {
    }
};

class Level4 : public Level3 {
    ENABLE_RETTR_CAST(Level3)
public:
    virtual ~Level4() {
    }
};

class Level5 : public Level4 {
    ENABLE_RETTR_CAST(Level4)
public:
    virtual ~Level5() {
    }
};

class GrandBase {
    ENABLE_RETTR_CAST()
public:
    virtual ~GrandBase() {
    }
    int grandData = 1;
};

class Base1 : virtual public GrandBase {
    ENABLE_RETTR_CAST(GrandBase)
public:
    int data1 = 10;
};

class Base2 : virtual public GrandBase {
    ENABLE_RETTR_CAST(GrandBase)
public:
    int data2 = 20;
};

class Derived : public Base1, public Base2 {
    ENABLE_RETTR_CAST(Base2) // 应该只能转换到Base2
public:
    int derivedData = 100;
};

class VBase {
    ENABLE_RETTR_CAST()
public:
    virtual ~VBase() {
    }
};

class VLevel1 : virtual public VBase {
    ENABLE_RETTR_CAST(VBase)
};

class VLevel2 : virtual public VLevel1 {
    ENABLE_RETTR_CAST(VLevel1)
};

class VLevel3 : public VLevel2 { // 混合
    ENABLE_RETTR_CAST(VLevel2)
};

RETTR_REGISTRATION {
    using namespace rettr;

    registration::class_<GrandBase>("GrandBase").constructor<>();
    registration::class_<Base1>("Base1").constructor<>();
    registration::class_<Base2>("Base2").constructor<>();
    registration::class_<Derived>("Derived").constructor<>();

    registration::class_<VBase>("VBase").constructor<>();
    registration::class_<VLevel1>("VLevel1").constructor<>();
    registration::class_<VLevel2>("VLevel2").constructor<>();
    registration::class_<VLevel3>("VLevel3").constructor<>();

    registration::class_<Base>("Base").constructor<>();
    registration::class_<Level1>("Level1").constructor<>();
    registration::class_<Level2>("Level2").constructor<>();
    registration::class_<Level3>("Level3").constructor<>();
    registration::class_<Level4>("Level4").constructor<>();
    registration::class_<Level5>("Level5").constructor<>();


    registration::class_<MyClass>("MyClass")(metadata(std::string_view{"Type"}, 111))
        .constructor<int, int, bool>()(parameter_names("left_operand", "right_operand", "add_one"), default_arguments(false))
        .constructor()
        .constructor<const MyClass&>()
        .method("hello", &MyClass::hello)
        .method("add", &MyClass::add)(parameter_names("left_operand", "right_operand", "add_one"), default_arguments(false))
        .property("field", &MyClass::field)(metadata("attr", 10));
}

int main() {
    {
        Derived *derivedObj = new Derived();
        GrandBase *grandPtr = derivedObj;

        Base2 *p2 = rettr::rettr_cast<Base2 *>(grandPtr);
        std::cout << "1. GrandBase -> Base2 (已注册): " << (p2 ? "成功" : "失败") << '\n';

        Base1 *p1 = rettr::rettr_cast<Base1 *>(grandPtr);
        std::cout << "2. GrandBase -> Base1 (未注册但有路径): " << (p1 ? "成功" : "失败") << '\n';

        Derived *d1 = rettr::rettr_cast<Derived *>(grandPtr);
        std::cout << "3. GrandBase -> Derived (反向转换): " << (d1 ? "成功" : "失败") << '\n';

        Base1 *b1 = derivedObj;
        Base2 *cross = rettr::rettr_cast<Base2 *>(b1);
        std::cout << "4. Base1 -> Base2 (交叉转换，通过虚基类): " << (cross ? "成功" : "失败") << '\n';

        Level5 *level5 = new Level5();
        Base *baseToLevel3 = rettr::rettr_cast<Base *>(level5);
        Level3 *level3Back = rettr::rettr_cast<Level3 *>(baseToLevel3);
        std::cout << "5. Level5 -> Base -> Level3 (多级跳转): " << (level3Back ? "成功" : "失败") << '\n';

        Level3 *wrong = rettr::rettr_cast<Level3 *>(derivedObj);
        std::cout << "6. Derived -> Level3 (无关类型): " << (wrong ? "不该成功" : "正确失败") << '\n';

        delete derivedObj;
        delete level5;
    }

    rettr::type t = rettr::type::from<MyClass>();

    auto meta = t.metadata("Type");

    std::cout << meta << '\n';

    MyClass object;

    std::ignore = t.invoke("hello", object);

    std::cout << t.property("field")(object) << '\n';
    std::cout << t.property("field").value(object) << '\n';

    t.property("field")(object) = 666;

    std::cout << t.property("field")(object) << '\n';
    std::cout << t.property("field").value(object) << '\n';

    t.property("field").value(object, 123);

    std::cout << t.property("field")(object) << '\n';
    std::cout << t.property("field").value(object) << '\n';

    auto prop = t.property("field");

    std::cout << prop.metadata("attr").value() << '\n';

    const auto &met = t.method("add");

    rettr::array_range<rettr::parameter_info> param_infos;
    {
        param_infos = t.constructor({rettr_typeid(int), rettr_typeid(int), rettr_typeid(bool)}).parameter_infos();
    }

    for (const auto &param: param_infos) {
        std::cout << "param " << param.name() << " type: " << param.type().name()
                  << " default_value: " << (param.has_default_value() ? param.default_value() : "none") << '\n';
    }

    auto obj = t.create();
    std::cout << obj.type().name() << '\n';

    rettr::object o = t.create_object();

    std::cout << o("field") << '\n';

    o("field") = 100;

    std::cout << (o("field") + 200) << '\n';

    o("field") += 100;

    o("field").prop() -= 100;

    std::cout << o("field") << '\n';

    o("hello")();

    rettr::object_view obj_view = o;

    obj_view("hello")();

    obj_view("field") = 666;

    std::cout << o("field") << '\n';

    obj_view("field") = 3.14f;

    std::cout << o("field") << '\n';

    auto so = obj_view.create_object();

    std::cout << so.type().name() << '\n';

    return 0;
}
