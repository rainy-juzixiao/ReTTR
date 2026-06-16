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
#define ANKERL_NANOBENCH_IMPLEMENT

#include "test_classes.hpp"

#include <rettr/rettr.hpp>

#include <nanobench.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace rettr;

#define ITEM_COUNT 10

template <typename T>
struct bench_data {
    std::vector<std::shared_ptr<T>> m_vec;
};

using bench_data_single_base = bench_data<ClassSingleBase>;

bench_data_single_base setup_level_1() {
    std::vector<std::shared_ptr<ClassSingleBase>> vec;
    vec.reserve(ITEM_COUNT * 5);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_shared<ClassSingle1A>());
        vec.push_back(std::make_shared<ClassSingle1B>());
        vec.push_back(std::make_shared<ClassSingle1C>());
        vec.push_back(std::make_shared<ClassSingle1D>());
        vec.push_back(std::make_shared<ClassSingle1E>());
    }

    return bench_data_single_base{std::move(vec)};
}

bench_data_single_base setup_level_3() {
    std::vector<std::shared_ptr<ClassSingleBase>> vec;
    vec.reserve(ITEM_COUNT * 5);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<ClassSingle3A>());
        vec.push_back(std::make_unique<ClassSingle3B>());
        vec.push_back(std::make_unique<ClassSingle3C>());
        vec.push_back(std::make_unique<ClassSingle3D>());
        vec.push_back(std::make_unique<ClassSingle3E>());
    }

    return bench_data_single_base{std::move(vec)};
}

bench_data_single_base setup_level_6() {
    std::vector<std::shared_ptr<ClassSingleBase>> vec;
    vec.reserve(ITEM_COUNT * 5);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<ClassSingle6A>());
        vec.push_back(std::make_unique<ClassSingle6B>());
        vec.push_back(std::make_unique<ClassSingle6C>());
        vec.push_back(std::make_unique<ClassSingle6D>());
        vec.push_back(std::make_unique<ClassSingle6E>());
    }

    return bench_data_single_base{std::move(vec)};
}

using bench_data_virtual = bench_data<ClassDiamondTop>;

bench_data_virtual setup_virtual_inheritance_level_1() {
    std::vector<std::shared_ptr<ClassDiamondTop>> vec;
    vec.reserve(ITEM_COUNT * 3);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<ClassDiamondLeft1>());
        vec.push_back(std::make_unique<ClassDiamondMiddle1>());
        vec.push_back(std::make_unique<ClassDiamondRight1>());
    }

    return bench_data_virtual{std::move(vec)};
}

bench_data_virtual setup_virtual_inheritance_level_3() {
    std::vector<std::shared_ptr<ClassDiamondTop>> vec;
    vec.reserve(ITEM_COUNT * 3);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<ClassDiamondLeft3>());
        vec.push_back(std::make_unique<ClassDiamondMiddle3>());
        vec.push_back(std::make_unique<ClassDiamondRight3>());
    }

    return bench_data_virtual{std::move(vec)};
}

bench_data_virtual setup_virtual_inheritance_level_6() {
    std::vector<std::shared_ptr<ClassDiamondTop>> vec;
    vec.reserve(ITEM_COUNT * 3);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<ClassDiamondLeft6>());
        vec.push_back(std::make_unique<ClassDiamondMiddle6>());
        vec.push_back(std::make_unique<ClassDiamondRight6>());
    }

    return bench_data_virtual{std::move(vec)};
}

bench_data<ClassMultipleBaseC> setup_multiple_class_hierachy() {
    std::vector<std::shared_ptr<ClassMultipleBaseC>> vec;
    vec.reserve(ITEM_COUNT * 7);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<FinalClass>());
        vec.push_back(std::make_unique<ClassMultiple5C>());
        vec.push_back(std::make_unique<FinalClass>());
        vec.push_back(std::make_unique<ClassMultiple4C>());
        vec.push_back(std::make_unique<FinalClass>());
        vec.push_back(std::make_unique<ClassMultiple5C>());
        vec.push_back(std::make_unique<FinalClass>());
    }

    return bench_data<ClassMultipleBaseC>{std::move(vec)};
}

bench_data<ClassMultiple3A> setup_cross_cast_class_hierachy_1() {
    std::vector<std::shared_ptr<ClassMultiple3A>> vec;
    vec.reserve(ITEM_COUNT * 2);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<FinalClass>());
        vec.push_back(std::make_unique<ClassMultiple6A>());
    }

    return bench_data<ClassMultiple3A>{std::move(vec)};
}

bench_data<ClassMultiple6A> setup_cross_cast_class_hierachy_2() {
    std::vector<std::shared_ptr<ClassMultiple6A>> vec;
    vec.reserve(ITEM_COUNT * 2);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        vec.push_back(std::make_unique<FinalClass>());
        vec.push_back(std::make_unique<ClassMultiple6A>());
    }

    return bench_data<ClassMultiple6A>{std::move(vec)};
}

void start_rettr_cast_benchmark() {
    ankerl::nanobench::Bench bench;
    bench.title("dynamic_cast vs. rettr_cast");
    bench.minEpochIterations(5000);

    // Helper lambda to run a benchmark with a setup and measurement function
    auto run_benchmark = [&](const std::string& name, auto setup_func, auto measure_func) {
        bench.run(name, [&]() {
            auto data = setup_func();
            volatile std::size_t value = 0;
            measure_func(data, value);
            bench.doNotOptimizeAway(value);
        });
    };

    // Group 1: downcast to level 1
    bench.title("downcast to level 1 - A cast from the base class one hierarchy level down.");
    auto setup_level1 = []() { return setup_level_1(); };
    auto measure_dynamic_cast_level1 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassSingle1A*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle1B*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle1C*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle1D*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle1E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_level1 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassSingle1A*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle1B*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle1C*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle1D*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle1E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_level1();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_level1(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_level1();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_level1(data, value));
    });

    // Group 2: downcast to level 3
    bench.title("downcast to level 3 - A cast from the base class three hierarchy levels down.");
    auto setup_level3 = []() { return setup_level_3(); };
    auto measure_dynamic_cast_level3 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassSingle3A*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle3B*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle3C*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle3D*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle3E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_level3 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassSingle3A*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle3B*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle3C*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle3D*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle3E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_level3();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_level3(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_level3();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_level3(data, value));
    });

    // Group 3: downcast to level 6
    bench.title("downcast to level 6 - A cast from the base class six hierarchy levels down.");
    auto setup_level6 = []() { return setup_level_6(); };
    auto measure_dynamic_cast_level6 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassSingle6A*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle6B*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle6C*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle6D*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassSingle6E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_level6 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassSingle6A*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle6B*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle6C*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle6D*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassSingle6E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_level6();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_level6(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_level6();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_level6(data, value));
    });

    // Group 4: virtual inheritance level 1
    bench.title("virtual inheritance level 1 - A cast from virtual base class one hierarchy levels down.");
    auto setup_virtual1 = []() { return setup_virtual_inheritance_level_1(); };
    auto measure_dynamic_cast_virtual1 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassDiamondLeft1*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassDiamondMiddle1*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassDiamondRight1*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_virtual1 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassDiamondLeft1*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassDiamondMiddle1*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassDiamondRight1*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_virtual1();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_virtual1(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_virtual1();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_virtual1(data, value));
    });

    // Group 5: virtual inheritance level 3
    bench.title("virtual inheritance level 3 - A cast from virtual base class three hierarchy levels down.");
    auto setup_virtual3 = []() { return setup_virtual_inheritance_level_3(); };
    auto measure_dynamic_cast_virtual3 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassDiamondLeft3*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassDiamondMiddle3*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassDiamondRight3*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_virtual3 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassDiamondLeft3*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassDiamondMiddle3*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassDiamondRight3*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_virtual3();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_virtual3(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_virtual3();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_virtual3(data, value));
    });

    // Group 6: virtual inheritance level 6
    bench.title("virtual inheritance level 6 - A cast from virtual base class six hierarchy levels down.");
    auto setup_virtual6 = []() { return setup_virtual_inheritance_level_6(); };
    auto measure_dynamic_cast_virtual6 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassDiamondLeft6*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassDiamondMiddle6*>(data.m_vec[i].get())) {
                value += 1;
            } else if (dynamic_cast<ClassDiamondRight6*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_virtual6 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassDiamondLeft6*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassDiamondMiddle6*>(data.m_vec[i].get())) {
                value += 1;
            } else if (rettr_cast<ClassDiamondRight6*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_virtual6();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_virtual6(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_virtual6();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_virtual6(data, value));
    });

    // Group 7: multiple inheritance
    bench.title("multiple inheritance - A cast from virtual base class to the most derived class,<br>which itself is derived from 5 other classes (all 7 hierarchy levels deep).");
    auto setup_multiple = []() { return setup_multiple_class_hierachy(); };
    auto measure_dynamic_cast_multiple = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<FinalClass*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_multiple = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<FinalClass*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_multiple();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_multiple(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_multiple();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_multiple(data, value));
    });

    // Group 8: cross cast 1
    bench.title("cross cast 1 - A cross cast in the 4th hierarchy level (out of 8)<br>to the third (out of 5 class wide hierarchy).");
    auto setup_cross1 = []() { return setup_cross_cast_class_hierachy_1(); };
    auto measure_dynamic_cast_cross1 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassMultiple3C*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_cross1 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassMultiple3C*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_cross1();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_cross1(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_cross1();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_cross1(data, value));
    });

    // Group 9: cross cast 2
    bench.title("cross cast 2 - A cross cast in the 7th hierarchy level (out of 8)<br>to the fifth (out of 5 class wide hierarchy).");
    auto setup_cross2 = []() { return setup_cross_cast_class_hierachy_2(); };
    auto measure_dynamic_cast_cross2 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (dynamic_cast<ClassMultiple6E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };
    auto measure_rettr_cast_cross2 = [](auto& data, volatile std::size_t& value) {
        for (std::size_t i = 0; i < data.m_vec.size(); ++i) {
            if (rettr_cast<ClassMultiple6E*>(data.m_vec[i].get())) {
                value += 1;
            }
        }
        return value;
    };

    bench.run("dynamic_cast", [&]() {
        auto data = setup_cross2();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_dynamic_cast_cross2(data, value));
    });
    bench.run("rettr_cast", [&]() {
        auto data = setup_cross2();
        volatile std::size_t value = 0;
        bench.doNotOptimizeAway(measure_rettr_cast_cross2(data, value));
    });
}