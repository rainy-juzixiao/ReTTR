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
module;
#include <rettr/any.hpp>

export module rettr.any;

export namespace rettr {
    /* main stub */
    using rettr::any;

    /* 转换器模板 */
    using rettr::any_converter;

    /* factory函数 */
    using rettr::make_any;

    /* match自动推导 */
    using rettr::auto_deduce_t;
    using rettr::auto_deduce;
}