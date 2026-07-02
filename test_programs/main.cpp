#include <iostream>
#include <random>
#include <rettr/registration.hpp>
#include <rettr/rettr_cast.hpp>
#include <rettr/rettr_enable.hpp>

#include <rettr/rettr.hpp>

#include <rettr/annotations/lunar/metadata.hpp>

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
    RETTR_ENABLE()
public:
    virtual ~Base() {
    }
};

class Level1 : public Base {
    RETTR_ENABLE(Base)
public:
    virtual ~Level1() {
    }
};

class Level2 : public Level1 {
    RETTR_ENABLE(Level1)
public:
    virtual ~Level2() {
    }
};

class Level3 : public Level2 {
    RETTR_ENABLE(Level2)
public:
    virtual ~Level3() {
    }
};

class Level4 : public Level3 {
    RETTR_ENABLE(Level3)
public:
    virtual ~Level4() {
    }
};

class Level5 : public Level4 {
    RETTR_ENABLE(Level4)
public:
    virtual ~Level5() {
    }
};

class GrandBase {
    RETTR_ENABLE()
public:
    virtual ~GrandBase() {
    }
    int grandData = 1;
};

class Base1 : virtual public GrandBase {
    RETTR_ENABLE(GrandBase)
public:
    int data1 = 10;
};

class Base2 : virtual public GrandBase {
    RETTR_ENABLE(GrandBase)
public:
    int data2 = 20;
};

class Derived : public Base1, public Base2 {
    RETTR_ENABLE(Base2) // 应该只能转换到Base2
public:
    int derivedData = 100;
};

class VBase {
    RETTR_ENABLE()
public:
    virtual ~VBase() {
    }
};

class VLevel1 : virtual public VBase {
    RETTR_ENABLE(VBase)
};

class VLevel2 : virtual public VLevel1 {
    RETTR_ENABLE(VLevel1)
};

class VLevel3 : public VLevel2 { // 混合
    RETTR_ENABLE(VLevel2)
};

namespace ns_3d {
    class node {
    public:
        node(std::string name, node *parent = nullptr) : m_parent(parent), m_name(std::move(name)) {
            if (m_parent) {
                m_parent->m_children.push_back(this);
            }
        }

        virtual ~node() {
            if (m_parent) {
                auto &siblings = m_parent->m_children;
                siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
            }
            for (node *child: m_children) {
                delete child;
            }
        }

        void set_name(const std::string &name) {
            m_name = name;
        }

        const std::string &get_name() const {
            return m_name;
        }

        std::vector<node *> get_children() const {
            return m_children;
        }

        void set_visible(bool visible, bool cascade = true) {
            // 简单实现，需要配合 RETTR 系统
            if (cascade) {
                for (node *child: m_children) {
                    child->set_visible(visible, cascade);
                }
            }
        }

        virtual void render() {
        }

    private:
        node *m_parent;
        std::string m_name;
        std::vector<node *> m_children;

        RETTR_ENABLE()
        RETTR_REGISTRATION_FRIEND
    };

    class mesh : public node {
    public:
        enum class render_mode {
            POINTS,
            WIREFRAME,
            SOLID
        };

        static mesh create_mesh(std::string file_name) {
            std::string name = file_name;
            size_t last_slash = name.find_last_of("/\\");
            if (last_slash != std::string::npos) {
                name = name.substr(last_slash + 1);
            }
            size_t last_dot = name.find_last_of('.');
            if (last_dot != std::string::npos) {
                name = name.substr(0, last_dot);
            }
            mesh m(name, nullptr);
            return m;
        }

        virtual void render() override {
            node::render();
            switch (m_render_mode) {
                case render_mode::POINTS:
                    // 渲染点为点云
                    break;
                case render_mode::WIREFRAME:
                    // 渲染为线框
                    break;
                case render_mode::SOLID:
                    // 渲染为实体
                    break;
            }
        }

        void set_render_mode(render_mode mode) {
            m_render_mode = mode;
        }

        render_mode get_render_mode() const {
            return m_render_mode;
        }

    private:
        mesh(std::string name, node *parent = nullptr) : node(std::move(name), parent), m_render_mode(render_mode::SOLID) {
        }

        render_mode m_render_mode;

        RETTR_ENABLE(node)
    };
}

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace xxx {
    class MyAnnoTestBase {
        RETTR_ENABLE()
    public:
        [[= rettr::annotations::metadata<"Version22", 2>()]] void method2() {
        }
    };

    class MyAnnoTest : public MyAnnoTestBase {
        RETTR_ENABLE(MyAnnoTestBase)
    public:
        [[= rettr::annotations::metadata<"Constructor", true>()]] MyAnnoTest create_anno_test(int) {
            return {};
        }

        [[= rettr::annotations::metadata<"default_construct", true>()]] MyAnnoTest() = default;

        [[= rettr::annotations::metadata<"default_construct", true>()]] MyAnnoTest(std::string name) {
        }

        [[ = rettr::annotations::metadata<"TIP", 3>(), = rettr::annotations::metadata<"TIP1", 3.14f>() ]] int value;

        [[ = rettr::annotations::metadata<"Version", 1>(), = rettr::annotations::metadata<"Version2", 111>() ]] void method1() {
        }

        [[= rettr::annotations::metadata<"Version", 2>()]] void method1(int value) {
        }
    };

    enum class[[= rettr::annotations::metadata<"Version", 250>()]] enums {
        value1 = 1,
        value2 = 2
    };

    struct MyStruct {
        MyStruct() {};

        explicit MyStruct(int data) : data(data) {
        }

        [[= rettr::annotations::mark_as_constructor_func]] MyStruct create_my_structure(std::string) {
            return {};
        }

        void func(double) {};

        int data;

    private:
        int data2;
    };
}

#endif

RETTR_REGISTRATION {
    using namespace rettr;
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

    registration::class_<xxx::MyAnnoTest>("MyAnnoTest")
        .constructor(&xxx::MyAnnoTest::create_anno_test)
        .constructor<std::string>()
        .constructor()
        .property("value", &xxx::MyAnnoTest::value)
        .method("method1", select_overload<xxx::MyAnnoTest, void()>(&xxx::MyAnnoTest::method1))
        .method("method1", select_overload<xxx::MyAnnoTest, void(int)>(&xxx::MyAnnoTest::method1))
        .method("method2", select_overload<xxx::MyAnnoTestBase, void()>(&xxx::MyAnnoTestBase::method2))
        .method("free_function", [](int value) { std::cout << "free_function" << '\n'; });

    registration::enumeration<xxx::enums>("enums");

    registration::class_<xxx::MyStruct>("MyStruct").make_this_available().make_member_data_available(registration::private_access);

#endif

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
        .constructor<const MyClass &>()
        .method("hello", &MyClass::hello)
        .method("add", &MyClass::add)(parameter_names("left_operand", "right_operand", "add_one"), default_arguments(false))
        .property("field", &MyClass::field)(metadata("attr", 10));

    using namespace ns_3d;
    registration::class_<node>("ns_3d::node")
        .constructor<std::string, node *>()(default_arguments(nullptr)
                                            // 第二个参数是可选的，在此处，我们为其提供一个默认值进行注入。
                                            )
        .property("name", &node::get_name, &node::set_name)(metadata("TOOL_TIP", "Set the name of node.")
                                                            // 将元数据挂载到name这个属性
                                                            )
        // 直接注册一个成员对象指针；将其标记为“私有”成员
        .property("parent", &ns_3d::node::m_parent, registration::private_access)
        .property_readonly("children", &node::get_children)
        // 这是一个只读属性；将会强制挂载为const对象以便无法修改
        .method("set_visible", &node::set_visible)(
            default_arguments(true), // the default value for 'cascade'
            parameter_names("visible", "cascade") // provide the names of the parameter; optional, but might be useful for clients
            )
        .method("render", &node::render);
    registration::class_<mesh>("ns_3d::mesh")
        .constructor(&mesh::create_mesh)
        .property("render_mode", &mesh::get_render_mode, &mesh::set_render_mode)
        .enumeration<mesh::render_mode>("ns_3d::render_mode")(value("POINTS", mesh::render_mode::POINTS),
                                                              value("WIREFRAME", mesh::render_mode::WIREFRAME),
                                                              value("SOLID", mesh::render_mode::SOLID));
}

int main() {
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
    {
        std::cout << rettr::type::from<xxx::MyStruct>().constructors().size() << '\n';
        std::cout << rettr::type::from<xxx::MyStruct>().constructors()[0].parameter_count() << '\n';
        std::cout << rettr::type::from<xxx::MyStruct>().methods().size() << '\n';
    }
    {
        {
            auto members = rettr::implements::entity::make_method_entites<xxx::MyAnnoTest>();

            template for (auto member: members) {
                std::cout << member.name_ptr << '\n';
            }
        }
        {
            auto t = rettr::type::from<xxx::enums>();

            std::cout << t.enumeration().metadatas().size() << '\n';

            for (const auto &item: t.metadatas()) {
                std::cout << item.key() << " : " << item.value() << '\n';
            }
            auto enum_ = t.enumeration();
            //
            for (const auto &enum_item: enum_.values()) {
                std::cout << (int) enum_item.as<xxx::enums>() << '\n';
            }
        }

        auto t = rettr::type::from<xxx::MyAnnoTest>();
        auto metadatas = t.property("value").metadatas();
        for (const auto &item: metadatas) {
            std::cout << item.key() << ' ' << item.value() << '\n';
        }

        for (const auto &method: t.methods()) {
            std::cout << "method " << method.name() << " : " << method.function_signature().name() << '\n';
            for (const auto &md: method.metadatas()) {
                std::cout << '\t' << md.key() << " : " << md.value() << '\n';
            }
            for (const auto &parameter_info: method.parameter_infos()) {
                std::cout << parameter_info.name() << '\n';
            }
        }

        auto ctor = t.constructor();

        std::cout << ctor.metadatas().size() << '\n';

        auto ctor_func = t.constructor({rettr_typeid(int)});
        std::cout << ctor_func.metadatas().size() << '\n';
        std::cout << ctor_func.parameter_infos().at(0).name() << '\n';

        {
            ctor_func = t.constructor({rettr_typeid(std::string)});
            std::cout << ctor_func.parameter_infos()[0].name() << '\n';
        }
    }
#endif
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
