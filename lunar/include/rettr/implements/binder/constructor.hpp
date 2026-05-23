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
#include <rettr/implements/binder/parameter.hpp>
#include <rettr/implements/parameter_info/wrapper.hpp>

namespace rettr::implements {
    template <typename Type, typename... CtorArgs>
    class constructor_bind {
    public:
        static constexpr std::size_t arity = sizeof...(CtorArgs);

        using invoker = constructor_invoker<Type, CtorArgs...>;
        using traits = function_traits<invoker>;

        template <std::size_t... Is>
        struct wrapper_tuple_impl {
            using type = std::tuple<parameter_info_wrapper<std::tuple_element_t<Is, std::tuple<CtorArgs...>>, Is, true, void>...>;
        };

        // NOLINTBEGIN

        template <std::size_t... Is>
        static auto make_wrapper_tuple_type(std::index_sequence<Is...>) -> wrapper_tuple_impl<Is...>;

        // NOLINTEND

        using wrapper_tuple_t = typename decltype(make_wrapper_tuple_type(std::make_index_sequence<arity>{}))::type;

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
        wrapper_tuple_t init_wrappers_(std::index_sequence<Is...>) {
            return wrapper_tuple_t{(void(Is), string_view{})...};
        }

        template <std::size_t N>
        void apply_(parameter_names_tag<N> &&tag) {
            static_assert(N == arity);
            apply_names_(tag.names, std::make_index_sequence<arity>{});
        }

        template <std::size_t N, std::size_t... Is>
        void apply_names_(const std::array<string_view, N> &names, std::index_sequence<Is...>) {
            wrappers_ = wrapper_tuple_t{names[Is]...};
        }

        template <typename... Defaults>
        void apply_(default_arguments_tag<Defaults...> &&tag) {
            static_assert(sizeof...(Defaults) <= arity);
            defaults_applier_ = [this, vals = std::move(tag.values)](function &fn) mutable {
                using implemented_type = typename get_ia_implement_type<invoker, default_arguments_store<Defaults...>, traits>::type;
                auto *impl = static_cast<implemented_type *>(fn.invoke_accessor());
                apply_defaults_(impl, vals, std::make_index_sequence<sizeof...(Defaults)>{},
                                std::integral_constant<std::size_t, arity - sizeof...(Defaults)>{});
            };
        }

        void apply_(access_level_tag &&tag) {
            access_level_ = tag.value;
        }

        void apply_(rettr::metadata &&meta) {
            metadatas_.emplace_back(std::move(meta));
        }

        template <typename Impl, typename DefaultsTuple, std::size_t... Is, std::size_t Offset>
        void apply_defaults_(Impl *impl, DefaultsTuple &vals, std::index_sequence<Is...>,
                             std::integral_constant<std::size_t, Offset>) {
            (static_cast<parameter_info_wrapper<std::tuple_element_t<Offset + Is, std::tuple<CtorArgs...>>, Offset + Is, true,
                                                std::tuple_element_t<Is, DefaultsTuple>> *>(&std::get<Offset + Is>(wrappers_))
                 ->set_default_value(impl->storage.template get<Is>()),
             ...);
        }

        template <std::size_t... Is>
        void commit_impl_(std::index_sequence<Is...>) {
            std::array<parameter_info_base *, arity> ptrs{&std::get<Is>(wrappers_)...};

            function fn{invoker{}};
            if (defaults_applier_) {
                defaults_applier_(fn);
            }

            std::vector<parameter_info> params;
            params.reserve(arity);
            for (auto *p: ptrs) {
                params.emplace_back(p);
            }

            commit_(constructor{std::move(fn), access_level_, std::move(params), std::move(metadatas_)});
        }

        std::function<void(constructor)> commit_;
        std::function<void(function &)> defaults_applier_;
        wrapper_tuple_t wrappers_;
        access_levels access_level_{access_levels::public_access};
        std::vector<rettr::metadata> metadatas_;
        bool committed_{false};
    };
}

#endif
