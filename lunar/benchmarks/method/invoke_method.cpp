#include "methods.h"
#include <rettr/rettr.hpp>
#include <nanobench.h>

void bench_invoke_method()
{
    ankerl::nanobench::Bench bench;
    bench.title("invoke member method");
    bench.minEpochIterations(500'000);

    {
        ns_foo::method_class obj;
        bench.run("native void", [&]() {
            obj.method_0();
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("method_0");
        bench.run("rettr void", [&]() {
            m.invoke(obj);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        bench.run("native int", [&]() {
            auto result = obj.int_method();
            ankerl::nanobench::doNotOptimizeAway(result);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("int_method");
        bench.run("rettr int", [&]() {
            auto result = m.invoke(obj);
            ankerl::nanobench::doNotOptimizeAway(result);
        });
    }

    {
        ns_foo::method_class obj;
        bench.run("native string", [&]() {
            auto result = obj.string_method();
            ankerl::nanobench::doNotOptimizeAway(result);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("string_method");
        bench.run("rettr string", [&]() {
            auto result = m.invoke(obj);
            ankerl::nanobench::doNotOptimizeAway(result);
        });
    }

    {
        ns_foo::method_class obj;
        bench.run("native arg 1", [&]() {
            obj.void_method_arg_1(23);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("void_method_arg_1");
        bench.run("rettr arg 1", [&]() {
            m.invoke(obj, 23);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        bench.run("native arg 2", [&]() {
            obj.void_method_arg_2(23, 42);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("void_method_arg_2");
        bench.run("rettr arg 2", [&]() {
            m.invoke(obj, 23, 42);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        bench.run("native arg 3", [&]() {
            obj.void_method_arg_3(23, 42, 5);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("void_method_arg_3");
        bench.run("rettr arg 3", [&]() {
            m.invoke(obj, 23, 42, 5);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        bench.run("native arg 8", [&]() {
            obj.void_method_arg_8(1, 2, 3, 4, 5, 6, 7, 8);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("void_method_arg_8");
        std::vector<rettr::any> args = {1, 2, 3, 4, 5, 6, 7, 8};
        bench.run("rettr arg 8", [&]() {
            m.invoke_variadic(obj, args);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        int* ptr = nullptr;
        bench.run("native ptr arg", [&]() {
            obj.method_ptr_arg(ptr);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }

    {
        ns_foo::method_class obj;
        rettr::method m = rettr::type::from(obj).method("method_ptr_arg");
        int* ptr = nullptr;
        bench.run("rettr ptr arg", [&]() {
            m.invoke(obj, ptr);
            ankerl::nanobench::doNotOptimizeAway(obj);
        });
    }
}