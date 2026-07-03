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
#include <iostream>
#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/registration/registration_state_saver.hpp>
#include <rettr/library.hpp>

#include <cstdio>
#include <dlfcn.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

static bool is_absolute_path(const std::string &file_name) noexcept {
    return !file_name.empty() && file_name[0] == '/'; // NOLINT
}

static std::vector<std::string> platform_suffixes(const std::string &version) {
    std::vector<std::string> result;
#if RETTR_USING_LINUX
    result.emplace_back(version.empty() ? ".so" : ".so." + version);
#endif

#if RETTR_USING_MACOS
    if (!version.empty()) {
        result.emplace_back("." + version + ".bundle");
        result.emplace_back("." + version + ".dylib");
    } else {
        result.emplace_back(".bundle");
        result.emplace_back(".dylib");
    }
#endif

    return result;
}

static bool file_exists(const std::string &path) noexcept {
    if (auto *file = std::fopen(path.c_str(), "r")) {
        std::fclose(file);
        return true;
    }
    return false;
}

static std::string dl_error() noexcept {
    const char *err = ::dlerror();
    return err ? std::string(err) : std::string{};
}

namespace rettr {
    class library::library_private {
    public:
        library_private(std::string_view file_name, std::string_view version) :
            file_name_(file_name.data(), file_name.size()), version_{version.data(), version.size()} {
            if (!version.empty()) {
                ;
            }
        }

        ~library_private() {
            if (handle_) {
                while (load_count_ > 0) {
                    unload_native();
                    --load_count_;
                }
            }
        }

        library_private(const library_private &) = delete;
        library_private &operator=(const library_private &) = delete;

        bool load() {
            if (handle_) {
                ++load_count_;
                return true;
            }
            state_.save_state_begin();
            const bool ok = load_native();
            if (ok) {
                ++load_count_;
                state_.save_state_end();
            }
            return ok;
        }

        bool is_loaded() const noexcept {
            return handle_ != nullptr;
        }

        bool unload() {
            if (!handle_) {
                error_ = "Cannot unload library '" + file_name_ + "': not loaded";
                return false;
            }
            if (load_count_ > 1) {
                --load_count_;
                return true;
            }
            const bool ok = unload_native();
            if (ok) {
                handle_ = nullptr;
                load_count_ = 0;
                qualified_file_name_.clear();
                state_.reset();
            }
            return ok;
        }

        std::string_view file_name() const noexcept {
            return file_name_;
        }

        std::string_view error_string() const noexcept {
            return error_;
        }

        array_range<type> types() const noexcept {
            return state_.types();
        }

        array_range<property> global_properties() const noexcept {
            return state_.global_properties();
        }

        array_range<method> global_methods() const noexcept {
            return state_.global_methods();
        }

    private:
        bool load_native() {
            std::vector<std::string> prefix_list = {"lib", ""};
            std::vector<std::string> suffix_list = platform_suffixes(version_);
            suffix_list.emplace_back(); // 加一个空后缀

            constexpr int flags = RTLD_NOW | RTLD_GLOBAL;

            for (auto pi = 0u; !handle_ && pi < prefix_list.size(); ++pi) {
                for (auto si = 0u; !handle_ && si < suffix_list.size(); ++si) {
                    // NOLINTBEGIN
                    const auto &pfx = prefix_list[pi];
                    const auto &sfx = suffix_list[si];
                    // NOLINTEND

                    const std::string_view current{file_name_};
                    if (!pfx.empty() && current.find(pfx) == 0) {
                        continue;
                    }
                    if (!sfx.empty() && current.size() >= sfx.size() &&
                        current.compare(current.size() - sfx.size(), sfx.size(), sfx) == 0) {
                        continue;
                    }

                    const std::string candidate = pfx + file_name_ + sfx;

                    std::string attempt = candidate;
                    if (!is_absolute_path(candidate)) {
                        std::error_code ec;
                        auto abs = std::filesystem::absolute(candidate, ec);
                        if (!ec) {
                            attempt = abs.string();
                        }
                    }

                    handle_ = ::dlopen(attempt.c_str(), flags);
                    if (handle_) {
                        qualified_file_name_ = attempt;
                    }
                }
            }

            if (!handle_) {
                error_ = "Cannot load library '" + file_name_ + "': " + dl_error();
                return false;
            }

            error_.clear();
            return true;
        }

        bool unload_native() {
            if (::dlclose(handle_) != 0) {
                error_ = "Cannot unload library '" + file_name_ + "': " + dl_error();
                return false;
            }
            return true;
        }

        std::string file_name_;
        std::string version_;
        std::string qualified_file_name_;
        std::string error_;
        void *handle_ = nullptr;
        int load_count_ = 0;
        implements::registration_state_saver state_;
    };

    library::library(std::string_view file_name, std::string_view version) : private_(std::make_unique<library_private>(file_name, version)) {
    }

    library::~library() = default;

    std::string_view library::file_name() const noexcept {
        return private_->file_name();
    }

    std::string_view library::error_string() const noexcept {
        return private_->error_string();
    }

    array_range<type> library::types() const noexcept {
        return private_->types();
    }

    array_range<property> library::global_properties() const noexcept {
        return private_->global_properties();
    }

    array_range<method> library::global_methods() const noexcept {
        return private_->global_methods();
    }

    bool library::is_loaded() const noexcept {
        return private_->is_loaded();
    }

    bool library::load() {
        return private_->load();
    }

    bool library::unload() {
        return private_->unload();
    }
}
