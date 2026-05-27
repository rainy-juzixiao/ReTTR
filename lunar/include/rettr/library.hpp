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
#ifndef RETTR_LIBRARY_HPP
#define RETTR_LIBRARY_HPP

#include <rettr/array_range.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/string_view.hpp>

namespace rettr {
    class method;
    class property;
    class type;

    class RETTR_API library {
    public:
        library(string_view file_name, string_view version = {});

        library(const library &) = delete;
        library(library &&) = delete;

        ~library();

        string_view file_name() const noexcept;

        array_range<property> global_properties() const noexcept;
        array_range<method> global_methods() const noexcept;

        array_range<type> types() const noexcept;

        bool is_loaded() const noexcept;

        bool load();

        bool unload();

        string_view error_string() const noexcept;

    private:
        class library_private;

        std::unique_ptr<library_private> private_;
    };
}

#endif
