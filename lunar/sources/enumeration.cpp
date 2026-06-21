#include <iostream>
#include <rettr/enumeration.hpp>
#include <rettr/type.hpp>
namespace rettr {
    string_view enumeration::name() const noexcept {
        if (empty()) {
            return {};
        }
        return data_->name;
    }

    rettr::type enumeration::type() const noexcept {
        if (empty()) {
            return {};
        }
        return rettr::type::from_typeid(data_->enum_type);
    }

    rettr::type enumeration::underlying_type() const noexcept {
        if (empty()) {
            return {};
        }
        return rettr::type::from_typeid(data_->underlying_type);
    }

    rettr::type enumeration::declaring_type() const noexcept {
        if (empty()) {
            return {};
        }
        return rettr::type::from_typeid(data_->declaring_type.remove_cvref());
    }

    array_range<string_view> enumeration::names() const noexcept {
        if (empty()) {
            return {};
        }
        return {data_->names.data(), data_->names.size()};
    }

    RETTR_NODISCARD array_range<any> enumeration::values() const noexcept {
        if (empty()) {
            return {};
        }
        return array_range<any>(data_->values.data(), data_->values.size());
    }

    RETTR_NODISCARD const rettr::metadata_item &enumeration::metadata(const any &key) const noexcept {
        static const rettr::metadata_item empty{};
        if (this->empty()) {
            return empty;
        }
        // NOLINTBEGIN
        const auto it = std::find_if(data_->metadata.begin(), data_->metadata.end(),
                                     [&key](const rettr::metadata_item &meta) { return meta.key() == key; });
        return it != data_->metadata.end() ? *it : empty;
        // NOLINTEND
    }

    RETTR_NODISCARD array_range<rettr::metadata_item> enumeration::metadatas() const noexcept {
        if (empty()) {
            return {};
        }
        return {data_->metadata.data(), data_->metadata.size()};
    }

    any enumeration::name_to_value(string_view name) const {
        if (empty()) {
            return {};
        }
        for (std::size_t i = 0; i < data_->names.size(); ++i) {
            if (data_->names[i] == name) {
                return data_->values[i];
            }
        }
        return {};
    }

    string_view enumeration::value_to_name(const any &value) const {
        if (empty()) {
            return {};
        }
        for (std::size_t i = 0; i < data_->values.size(); ++i) {
            if (data_->values[i] == value) {
                return data_->names[i];
            }
        }
        return {};
    }
}
