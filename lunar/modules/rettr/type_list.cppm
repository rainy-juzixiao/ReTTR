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
#include <rettr/core/type_list.hpp>

export module rettr.type_list;

export namespace rettr::helper {
    using rettr::helper::type_list;
    using rettr::helper::type_list_concat;
    using rettr::helper::type_at;
    using rettr::helper::type_at_t;
    using rettr::helper::type_list_push_front;
    using rettr::helper::type_list_push_front_t;
    using rettr::helper::type_list_push_back;
    using rettr::helper::type_list_push_back_t;
    using rettr::helper::type_list_push_front_all;
    using rettr::helper::type_list_push_back_all;
    using rettr::helper::type_list_pop_front;
    using rettr::helper::type_list_pop_back;
    using rettr::helper::tuple_like_to_type_list;
    using rettr::helper::type_list_to_tuple_like;
    using rettr::helper::type_list_size_v;
    using rettr::helper::type_list_size;
    using rettr::helper::count_type_v;
    using rettr::helper::count_type;
    using rettr::helper::sub_type_list;
    using rettr::helper::type_list_front;
    using rettr::helper::type_find_unique;
    using rettr::helper::is_type_in_list;
    using rettr::helper::type_list_npos;
    using rettr::helper::type_list_quote;
    using rettr::helper::type_list_invoke;
    using rettr::helper::type_list_bind_back;
    using rettr::helper::type_list_apply;
    using rettr::helper::type_list_join;
    using rettr::helper::type_list_transform;
    using rettr::helper::type_list_cartesian_product;
    using rettr::helper::meta_as_integer_sequence;
    using rettr::helper::as_list;
    using rettr::helper::type_list_contains;
    using rettr::helper::unique_type_list;
    using rettr::helper::unique_type_list_t;
    using rettr::helper::select_type;
}
