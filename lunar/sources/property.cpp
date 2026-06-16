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
#include <algorithm>
#include <rettr/property.hpp>
#include <rettr/type.hpp>

namespace rettr {
    rettr::type property::declaring_type() const noexcept {
        return rettr::type::from_typeid(which_belongs().remove_cvref());
    }

    const typeinfo &property::which_belongs() const noexcept {
        return accessor_->which_belongs();
    }

    const typeinfo &property::property_type() const noexcept {
        return accessor_->property_type();
    }

    const typeinfo &property::compound_type() const noexcept {
        return accessor_->compound();
    }

    property_flags property::type() const noexcept {
        return accessor_->type();
    }

    bool property::is_const() const noexcept {
        return static_cast<bool>(type() & property_flags::const_property);
    }

    bool property::is_volatile() const noexcept {
        return static_cast<bool>(type() & property_flags::volatile_property);
    }

    bool property::is_member_pointer() const noexcept {
        return static_cast<bool>(type() & property_flags::member_property);
    }

    bool property::is_pointer() const noexcept {
        return property_type().is_pointer();
    }

    bool property::is_array() const noexcept {
        return property_type().has_traits(traits::is_array);
    }

    bool property::is_fundamental() const noexcept {
        return property_type().has_traits(traits::is_fundamental);
    }

    bool property::is_compound() const noexcept {
        return property_type().has_traits(traits::is_compound);
    }

    bool property::is_enumeration() const noexcept {
        return static_cast<bool>(type() & property_flags::enum_property);
    }

    bool property::is_readonly() const noexcept {
        return is_const();
    }

    property::operator bool() const noexcept {
        return !empty();
    }

    void property::clear() noexcept {
        accessor_.reset();
        name_ = {};
        is_empty_ = true;
        metadatas_.clear();
    }

    bool property::empty() const noexcept {
        return is_empty_;
    }

    const metadata_item &property::metadata(const any &key) const noexcept {
        static const metadata_item empty{};
        const auto it =
            std::find_if(metadatas_.begin(), metadatas_.end(), [&key](const rettr::metadata_item &m) { return m.key() == key; });
        return it != metadatas_.end() ? *it : empty;
    }

    array_range<rettr::metadata_item> property::metadatas() const noexcept {
        return {metadatas_.data(), metadatas_.size()};
    }

    string_view property::name() const noexcept {
        return name_;
    }

    access_levels property::access_level() const noexcept {
        return access_levels_;
    }

    any::reference property::value(object_view object) {
        return accessor_->get_property(object);
    }

    any::reference property::value(object_view object) const {
        return accessor_->get_property(object);
    }

    void property::value(object_view object, const any &val) const {
        accessor_->set_property(object, val);
    }

    property::property(const property &right) noexcept :
        name_(right.name_), metadatas_(right.metadatas_), access_levels_(right.access_levels_), is_empty_(right.is_empty_) {
        if (right.accessor_) {
            accessor_ = right.accessor_->clone();
        }
    }

    property::property(property &&right) noexcept :
        name_(right.name_), metadatas_(std::move(right.metadatas_)), access_levels_(right.access_levels_), is_empty_(right.is_empty_),
        accessor_(std::move(right.accessor_)) {
        right.is_empty_ = true;
    }

    property &property::operator=(const property &right) noexcept {
        if (this != &right) {
            name_ = right.name_;
            metadatas_ = right.metadatas_;
            access_levels_ = right.access_levels_;
            is_empty_ = right.is_empty_;
            accessor_ = right.accessor_ ? right.accessor_->clone() : nullptr;
        }
        return *this;
    }

    property &property::operator=(property &&right) noexcept {
        if (this != &right) {
            name_ = right.name_;
            metadatas_ = std::move(right.metadatas_);
            access_levels_ = right.access_levels_;
            is_empty_ = right.is_empty_;
            accessor_ = std::move(right.accessor_);
            right.is_empty_ = true;
        }
        return *this;
    }
}
