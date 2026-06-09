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
#include <rettr/rettr.hpp>

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

template class std::basic_string<char>;

#define RTTR_REGISTRATION_STANDARD_TYPE(T)                                                                                            \
    rettr::type::from<T>();                                                                                                           \
    rettr::type::from<T *>();                                                                                                         \
    rettr::type::from<const T *>();

// as-if == https://github.com/rttrorg/rttr/blob/master/src/rttr/detail/misc/standard_types.cpp

RETTR_REGISTRATION {
    using namespace rettr;

    type::from<std::nullptr_t>();

    RTTR_REGISTRATION_STANDARD_TYPE(void)
    RTTR_REGISTRATION_STANDARD_TYPE(rettr::type)
    RTTR_REGISTRATION_STANDARD_TYPE(bool)
    RTTR_REGISTRATION_STANDARD_TYPE(signed char)
    RTTR_REGISTRATION_STANDARD_TYPE(unsigned char)
    RTTR_REGISTRATION_STANDARD_TYPE(char)
    RTTR_REGISTRATION_STANDARD_TYPE(wchar_t)
    RTTR_REGISTRATION_STANDARD_TYPE(short int)
    RTTR_REGISTRATION_STANDARD_TYPE(unsigned short int)
    RTTR_REGISTRATION_STANDARD_TYPE(int)
    RTTR_REGISTRATION_STANDARD_TYPE(unsigned int)
    RTTR_REGISTRATION_STANDARD_TYPE(long int)
    RTTR_REGISTRATION_STANDARD_TYPE(unsigned long int)
    RTTR_REGISTRATION_STANDARD_TYPE(long long int)
    RTTR_REGISTRATION_STANDARD_TYPE(unsigned long long int)
    RTTR_REGISTRATION_STANDARD_TYPE(float)
    RTTR_REGISTRATION_STANDARD_TYPE(double)
    RTTR_REGISTRATION_STANDARD_TYPE(long double)
    RTTR_REGISTRATION_STANDARD_TYPE(std::string)

    registration::class_<std::vector<bool>>("std::vector<bool>");
    registration::class_<std::vector<int>>("std::vector<int>");
    registration::class_<std::vector<float>>("std::vector<float>");
    registration::class_<std::vector<double>>("std::vector<double>");

    registration::class_<std::string>("std::string")
        .constructor<>()
        .constructor<const std::string &>()
        .constructor<const std::string &, unsigned int, unsigned int>()
        .constructor<const char *>()
        .constructor<const char *, unsigned int>()
        .constructor<unsigned int, char>();
}
