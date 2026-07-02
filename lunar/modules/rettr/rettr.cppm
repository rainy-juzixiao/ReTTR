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
export module rettr;

// 基础支持类型
export import rettr.access_levels;
export import rettr.array_range;
export import rettr.enum_flags;
export import rettr.filter_item;
export import rettr.string_view;
export import rettr.type_list;

// 类型信息
export import rettr.typeinfo;
export import rettr.type;
export import rettr.any;

// 对象模型
export import rettr.object_view;
export import rettr.object;
export import rettr.shared_object;

// 反射核心
export import rettr.registration;
export import rettr.function;
export import rettr.method;
export import rettr.property;
export import rettr.constructor;
export import rettr.destructor;
export import rettr.enumeration;
export import rettr.parameter_info;
export import rettr.library;

// 转换支持
export import rettr.rettr_cast;
