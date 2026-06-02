#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include <rettr/rettr.hpp>
#include "methods.h"

void unit_1(ankerl::nanobench::Bench bench) {
    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr single hierarchy - Lvl. 1", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_1"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }
}

void bench_find_method()
{
    ankerl::nanobench::Bench bench;
    bench.title("find member method");
    bench.minEpochIterations(500'000);

    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        bench.run("native single hierarchy - Lvl. 1", [&]() {
            if (dynamic_cast<ns_foo::method_class_d_1*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    unit_1(bench);

    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        bench.run("native single hierarchy - Lvl. 3", [&]() {
            if (dynamic_cast<ns_foo::method_class_d_3*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr single hierarchy - Lvl. 3", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_3"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        bench.run("native single hierarchy - Lvl. 6", [&]() {
            if (dynamic_cast<ns_foo::method_class_d_6*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr single hierarchy - Lvl. 6", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_6"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        bench.run("native across hierarchy - Lvl. 1", [&]() {
            if (dynamic_cast<ns_foo::class_multiple_1C*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr across hierarchy - Lvl. 1", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_13"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        bench.run("native across hierarchy - Lvl. 3", [&]() {
            if (dynamic_cast<ns_foo::class_multiple_3C*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr across hierarchy - Lvl. 3", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_15"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        bench.run("native across hierarchy - Lvl. 6", [&]() {
            if (dynamic_cast<ns_foo::class_multiple_final*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr across hierarchy - Lvl. 6", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_18"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        bench.run("native neg. single hierarchy - Lvl. 1", [&]() {
            if (dynamic_cast<ns_foo::method_class_d_2*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_1 obj_1;
        ns_foo::method_class& obj_base = obj_1;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr neg. single hierarchy - Lvl. 1", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_2"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        bench.run("native neg. single hierarchy - Lvl. 3", [&]() {
            if (dynamic_cast<ns_foo::method_class_d_4*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_3 obj_3;
        ns_foo::method_class& obj_base = obj_3;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr neg. single hierarchy - Lvl. 3", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_4"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        bench.run("native neg. single hierarchy - Lvl. 6", [&]() {
            if (dynamic_cast<ns_foo::method_class_d_7*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::method_class_d_6 obj_6;
        ns_foo::method_class& obj_base = obj_6;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr neg. single hierarchy - Lvl. 6", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_7"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_2A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        bench.run("native neg. across hierarchy - Lvl. 1", [&]() {
            if (dynamic_cast<ns_foo::class_multiple_1C*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_2A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr neg. across hierarchy - Lvl. 1", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_13"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_3A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        bench.run("native neg. across hierarchy - Lvl. 3", [&]() {
            if (dynamic_cast<ns_foo::class_multiple_3C*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_3A obj_final;
        ns_foo::class_multiple_base_A& obj_base = obj_final;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr neg. across hierarchy - Lvl. 3", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_15"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        bench.run("native neg. across hierarchy - Lvl. 6", [&]() {
            if (dynamic_cast<ns_foo::class_multiple_final_D*>(&obj_base)) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }

    {
        ns_foo::class_multiple_final obj_final;
        ns_foo::class_multiple_base_C& obj_base = obj_final;
        volatile std::size_t value = 0;
        rettr::method m = rettr::type::from(nullptr).method("");
        bench.run("rettr neg. across hierarchy - Lvl. 6", [&]() {
            if ((m = rettr::type::from(obj_base).method("method_19"))) {
                value = 1;
            }
            ankerl::nanobench::doNotOptimizeAway(value);
        });
    }
}