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
#ifndef RETTR_IMPLEMENTS_BINDER_CONSTRUCTOR_HPP
#define RETTR_IMPLEMENTS_BINDER_CONSTRUCTOR_HPP

#include <rettr/constructor.hpp>
#include <rettr/implements/binder/access_levels.hpp>
#include <rettr/implements/binder/metadata.hpp>
#include <rettr/implements/binder/parameter.hpp>
#include <rettr/implements/parameter_info/wrapper.hpp>

namespace rettr::implements {
    template <typename Type, typename... CtorArgs>
    class constructor_bind {
    public:
        static constexpr std::size_t arity = sizeof...(CtorArgs);

        using invoker = constructor_invoker<Type, CtorArgs...>;
        using traits = function_traits<invoker>;

        explicit constructor_bind(std::function<void(constructor)> commit) noexcept :
            commit_(std::move(commit)), wrappers_(init_wrappers_(std::make_index_sequence<arity>{})) {
        }

        ~constructor_bind() {
            if (!committed_) {
                commit_impl_(std::make_index_sequence<arity>{});
            }
        }

        template <typename... Modifiers>
        constructor_bind &operator()(Modifiers &&...mods) {
            int dummy[] = {0, (apply_(std::forward<Modifiers>(mods)), 0)...};
            (void) dummy;
            commit_impl_(std::make_index_sequence<arity>{});
            committed_ = true;
            return *this;
        }

    private:
        template <std::size_t... Is>
        auto init_wrappers_(std::index_sequence<Is...>) {
            std::vector<std::unique_ptr<parameter_info_base>> result;
            result.reserve(arity);
            ((result.emplace_back(
                 std::make_unique<parameter_info_wrapper<std::tuple_element_t<Is, std::tuple<CtorArgs...>>, Is>>(string_view{}))),
             ...);
            return result;
        }

        template <std::size_t N>
        void apply_(parameter_names_tag<N> &&tag) {
            static_assert(N == arity);
            for (std::size_t i = 0; i < arity; ++i) {
                if (i < N) {
                    wrappers_[i]->set_name(tag.names[i]);
                }
            }
        }

        template <typename... Defaults>
        void apply_to_ctor(function &fn, std::tuple<Defaults...> &args) {
            std::apply(
                [&fn, this](auto &&...args) {
                    fn = {invoker{}, std::forward<decltype(args)>(args)...};
                },
                args);
        }

        template <typename... Defaults>
        void apply_(default_arguments_tag<Defaults...> &&tag) {
            static_assert(sizeof...(Defaults) <= arity);
            defaults_applier_ = [this, vals = std::move(tag.values)](function &fn) mutable {
                constexpr std::size_t num_defaults = sizeof...(Defaults);
                constexpr std::size_t offset = arity - num_defaults;

                apply_to_ctor(fn, vals);

                using implemented_type = typename get_ia_implement_type<invoker, default_arguments_store<Defaults...>, traits>::type;
                auto *impl = static_cast<implemented_type *>(fn.invoke_accessor());
                apply_defaults_(impl, vals, std::make_index_sequence<num_defaults>{}, std::integral_constant<std::size_t, offset>{});
            };
        }

        void apply_(access_level_tag &&tag) {
            access_level_ = tag.value;
        }

        void apply_(metadata_tag &&meta) {
            metadatas_.emplace_back(std::move(meta.value));
        }

        template <std::size_t I, std::size_t Offset, std::size_t NumDefaults, typename Impl, typename DefaultsTuple>
        void apply_one_default_(Impl *impl, DefaultsTuple &vals) {
            constexpr std::size_t param_index = Offset + I;
            any value(std::in_place_type<std::decay_t<std::tuple_element_t<I, DefaultsTuple>>>, std::get<I>(vals));
            wrappers_[param_index]->set_default_value(value);
        }

        template <typename Impl, typename DefaultsTuple, std::size_t... Is, std::size_t Offset>
        void apply_defaults_(Impl *impl, DefaultsTuple &vals, std::index_sequence<Is...>,
                             std::integral_constant<std::size_t, Offset>) {
            constexpr std::size_t num_defaults = sizeof...(Is);
            (apply_one_default_<Is, Offset, num_defaults, Impl, DefaultsTuple>(impl, vals), ...);
        }

        template <std::size_t... Is>
        void commit_impl_(std::index_sequence<Is...>) {
            function fn{};
            if (defaults_applier_) {
                defaults_applier_(fn);
            } else {
                fn = {invoker{}};
            }

            std::vector<parameter_info> params;
            params.reserve(arity);
            for (auto &p: wrappers_) {
                params.emplace_back(p->clone());
            }

            commit_(constructor{std::move(fn), access_level_, std::move(params), std::move(metadatas_)});
        }

        std::function<void(constructor)> commit_;
        std::function<void(function &)> defaults_applier_;
        std::vector<std::unique_ptr<parameter_info_base>> wrappers_;
        access_levels access_level_{access_levels::public_access};
        std::vector<rettr::metadata_item> metadatas_;
        bool committed_{false};
    };
}

namespace rettr::implements {
    template <typename Func>
    class constructor_func_bind {
    public:
        static constexpr std::size_t arity = function_traits<Func>::arity;

        using invoker = Func;
        using traits = function_traits<invoker>;

        explicit constructor_func_bind(std::function<void(constructor)> commit, Func &&func) noexcept :
            commit_(std::move(commit)), func_(std::forward<Func>(func)), wrappers_(init_wrappers_(std::make_index_sequence<arity>{})) {
        }

        ~constructor_func_bind() {
            if (!committed_) {
                commit_impl_(std::make_index_sequence<arity>{});
            }
        }

        template <typename... Modifiers>
        constructor_func_bind &operator()(Modifiers &&...mods) {
            int dummy[] = {0, (apply_(std::forward<Modifiers>(mods)), 0)...};
            (void) dummy;
            commit_impl_(std::make_index_sequence<arity>{});
            committed_ = true;
            return *this;
        }

    private:
        template <std::size_t... Is>
        auto init_wrappers_(std::index_sequence<Is...>) {
            std::vector<std::unique_ptr<parameter_info_base>> result;
            result.reserve(arity);
            ((result.emplace_back(
                 std::make_unique<parameter_info_wrapper<typename function_traits<Func>::template argument_type<Is>, Is>>(
                     string_view{}))),
             ...);
            return result;
        }

        template <std::size_t N>
        void apply_(parameter_names_tag<N> &&tag) {
            static_assert(N == arity);
            for (std::size_t i = 0; i < arity; ++i) {
                if (i < N) {
                    wrappers_[i]->set_name(tag.names[i]);
                }
            }
        }

        template <typename... Defaults>
        void apply_to_ctor(function &fn, std::tuple<Defaults...> &args) {
            std::apply(
                [&fn, this](auto &&...args) {
                    fn = {invoker{}, std::forward<decltype(args)>(args)...};
                },
                args);
        }

        template <typename... Defaults>
        void apply_(default_arguments_tag<Defaults...> &&tag) {
            static_assert(sizeof...(Defaults) <= arity);
            defaults_applier_ = [this, vals = std::move(tag.values)](function &fn) mutable {
                constexpr std::size_t num_defaults = sizeof...(Defaults);
                constexpr std::size_t offset = arity - num_defaults;

                apply_to_ctor(fn, vals);

                using implemented_type = typename get_ia_implement_type<invoker, default_arguments_store<Defaults...>, traits>::type;
                auto *impl = static_cast<implemented_type *>(fn.invoke_accessor());
                apply_defaults_(impl, vals, std::make_index_sequence<num_defaults>{}, std::integral_constant<std::size_t, offset>{});
            };
        }

        void apply_(access_level_tag &&tag) {
            access_level_ = tag.value;
        }

        void apply_(metadata_tag &&meta) {
            metadatas_.emplace_back(std::move(meta.value));
        }

        template <std::size_t I, std::size_t Offset, std::size_t NumDefaults, typename Impl, typename DefaultsTuple>
        void apply_one_default_(Impl *impl, DefaultsTuple &vals) {
            constexpr std::size_t param_index = Offset + I;
            auto val = std::get<I>(vals);
            any value(std::in_place_type<std::decay_t<std::tuple_element_t<I, DefaultsTuple>>>, val);
            wrappers_[param_index]->set_default_value(value);
        }

        template <typename Impl, typename DefaultsTuple, std::size_t... Is, std::size_t Offset>
        void apply_defaults_(Impl *impl, DefaultsTuple &vals, std::index_sequence<Is...>,
                             std::integral_constant<std::size_t, Offset>) {
            constexpr std::size_t num_defaults = sizeof...(Is);
            (apply_one_default_<Is, Offset, num_defaults, Impl, DefaultsTuple>(impl, vals), ...);
        }

        template <std::size_t... Is>
        void commit_impl_(std::index_sequence<Is...>) {
            function fn{};
            if (defaults_applier_) {
                defaults_applier_(fn);
            } else {
                fn = {std::move(func_)};
            }

            std::vector<parameter_info> params;
            params.reserve(arity);
            for (auto &p: wrappers_) {
                params.emplace_back(p->clone());
            }

            commit_(rettr::constructor{std::move(fn), access_level_, std::move(params), std::move(metadatas_)});
        }

        std::function<void(constructor)> commit_;
        Func func_;
        std::function<void(function &)> defaults_applier_;
        std::vector<std::unique_ptr<parameter_info_base>> wrappers_;
        access_levels access_level_{access_levels::public_access};
        std::vector<rettr::metadata_item> metadatas_;
        bool committed_{false};
    };
}

#endif
