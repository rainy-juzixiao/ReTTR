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
#include <catch2/catch_all.hpp>
#include <rettr/rettr.hpp>

using namespace rettr;

TEST_CASE("type name", "[type]")
{
   CHECK(type::from<int*>().name() == "int*");
   CHECK(type::from<const int*>().name() == "const int*");
   CHECK(type::from<const int&>().name() == "const int&");
   CHECK(type::from<const int**&>().name() == "const int**&");
}

TEST_CASE("Test rttr::type - type::from_name", "[type]")
{
    CHECK(type::from_name("std::string").empty()   == false);
    CHECK(type::from_name("std::string*").empty()  == true);
}
