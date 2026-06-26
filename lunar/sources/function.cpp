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
#include <rettr/function.hpp>

namespace rettr {
    function::function() noexcept = default;

    function::function(function &&right) noexcept {
        move_from_other(std::move(right));
    }

    function::function(const function &right) noexcept {
        copy_from_other(right);
    }

    function::function(std::nullptr_t) noexcept {
    }

    function::~function() {
        reset();
    }

    bool function::is_local() const noexcept {
        return invoke_accessor() == reinterpret_cast<const void *>(invoker_storage);
    }

    const typeinfo &function::return_type() const noexcept {
        assert(!empty()&& "You're trying to get the return type of a empty object!");
        return invoke_accessor()->return_type();
    }

    const array_range<typeinfo> &function::paramlists() const noexcept {
        assert(!empty()&& "You're trying to get the param list of a empty object!");
        return invoke_accessor()->paramlists();
    }

    bool function::empty() const noexcept {
        return invoke_accessor() == nullptr;
    }

    void function::copy_from_other(const function &right) noexcept {
        if (this == std::addressof(right) || right.empty()) {
            return;
        }
        invoke_accessor_ = right.invoke_accessor()->construct_from_this(this->invoker_storage);
    }

    void function::move_from_other(function &&right) noexcept {
        if (this == std::addressof(right) || right.empty()) {
            return;
        }
        if (right.is_local()) {
            invoke_accessor_ = right.invoke_accessor()->construct_from_this(this->invoker_storage);
            right.invoke_accessor_ = nullptr;
        } else {
            invoke_accessor_ = std::exchange(right.invoke_accessor_, nullptr);
        }
    }

    void function::swap(function &right) noexcept {
        if (this == std::addressof(right)) {
            return;
        }
        if (!is_local() && !right.is_local()) {
            std::swap(invoke_accessor_, right.invoke_accessor_);
        } else {
            function temp;
            temp.move_from_other(std::move(*this));
            this->move_from_other(std::move(right));
            right.move_from_other(std::move(temp));
        }
    }

    function &function::operator=(const function &right) noexcept {
        reset();
        if (this == std::addressof(right) || right.empty()) {
            return *this;
        }
        copy_from_other(right);
        return *this;
    }

    function &function::operator=(function &&right) noexcept {
        reset();
        if (this == std::addressof(right) || right.empty()) {
            return *this;
        }
        move_from_other(std::move(right));
        return *this;
    }

    function &function::operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    const typeinfo &function::function_signature() const noexcept {
        assert(!empty()&& "You're trying to get the function signature of a empty object!");
        return invoke_accessor()->function_signature();
    }

    bool function::has(const method_flags flag) const noexcept {
        return static_cast<bool>(type() & flag);
    }

    function::operator bool() const noexcept {
        return !empty();
    }

    bool function::equal_with(const function &right) const noexcept {
        if (this->empty() && right.empty()) {
            return true;
        }
        if (this->empty() || right.empty()) {
            return false;
        }
        return invoke_accessor()->equal_with(
            reinterpret_cast<const implements::invoker_accessor *>(right.invoker_storage));
    }

    bool function::not_equal_with(const function &right) const noexcept {
        return !equal_with(right);
    }

    void function::reset() noexcept {
        if (!empty()) {
            invoke_accessor()->destruct(is_local());
            invoke_accessor_ = nullptr;
        }
    }

    void function::rebind(function &&function) noexcept {
        move_from_other(std::move(function));
    }

    void function::rebind(const function &function) noexcept {
        copy_from_other(function);
    }

    void function::rebind(std::nullptr_t) noexcept {
        reset();
    }

    const typeinfo &function::which_belongs() const noexcept {
        assert(!empty());
        return invoke_accessor()->which_belongs();
    }

    method_flags function::type() const noexcept {
        assert(!empty()&& "You're trying to get the function type of a empty object!");
        return invoke_accessor()->type();
    }

    bool function::is_static() const noexcept {
        if (empty()) {
            return false;
        }
        return has(method_flags::static_specified);
    }

    bool function::is_memfn() const noexcept {
        if (empty()) {
            return false;
        }
        return !is_static();
    }

    bool function::is_volatile() const noexcept {
        if (empty()) {
            return false;
        }
        return has(method_flags::volatile_qualified);
    }

    bool function::is_const() const noexcept {
        if (empty()) {
            return false;
        }
        return has(method_flags::const_qualified);
    }

    bool function::is_noexcept() const noexcept {
        if (empty()) {
            return false;
        }
        return has(method_flags::noexcept_specified);
    }

    bool function::is_invoke_for_lvalue() const noexcept {
        if (empty()) {
            return false;
        }
        return has(method_flags::lvalue_qualified);
    }

    bool function::is_invoke_for_rvalue() const noexcept {
        if (empty()) {
            return false;
        }
        return has(method_flags::rvalue_qualified);
    }

    invocable_result function::is_invocable(const array_range<typeinfo> paramlist) const noexcept {
        if (empty()) {
            return invocable_result::failed;
        }
        return invoke_accessor()->is_invocable(paramlist);
    }

    std::size_t function::arity() const noexcept {
        assert(!empty()&&"You're trying to get the arg count of a empty object!");
        return invoke_accessor()->paramlists().size();
    }

    const typeinfo &function::arg(const std::size_t idx) const {
        assert(!empty()&& "You're trying to get the arg type of a empty object!");
        return invoke_accessor()->paramlists().at(static_cast<std::ptrdiff_t>(idx));
    }

    invocable_result function::is_variadic_invocable_with(const array_range<any> args) const {
        assert(!empty()&& "You're trying to get the arg type of a empty object!");
        return invoke_accessor()->is_invocable_with(args);
    }

    any function::invoke_variadic(object_view instance,
                                               const array_range<any> args) const {
        if (empty()) {
            throw rettr::bad_function_call();
        }
        return invoke_accessor()->dynamic_invoke(instance, args);
    }
}
