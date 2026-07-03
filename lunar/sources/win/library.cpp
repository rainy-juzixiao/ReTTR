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
#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/registration/registration_state_saver.hpp>
#include <rettr/library.hpp>

#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

static bool is_absolute_path(const std::string &file_name) noexcept {
    return !file_name.empty() && (file_name[0] == '/' || file_name[0] == '\\' ||
                                  (file_name.size() >= 2 && file_name[1] == ':' &&
                                   ((file_name[0] >= 'A' && file_name[0] <= 'Z') || (file_name[0] >= 'a' && file_name[0] <= 'z'))));
}

static std::vector<std::string> platform_suffixes(const std::string &version) {
    std::vector<std::string> result;
    if (!version.empty()) {
        result.emplace_back(".dll." + version);
    } else {
        result.emplace_back(".dll");
    }
    return result;
}

static std::string win_error() noexcept {
    DWORD error_code = ::GetLastError();
    if (error_code == 0) {
        return std::string{};
    }
    LPSTR message_buffer = nullptr;
    size_t size =
        ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
                         error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &message_buffer, 0, nullptr);
    std::string message(message_buffer, size);
    ::LocalFree(message_buffer);
    return message;
}

namespace rettr {
    class library::library_private {
    public:
        library_private(std::string_view file_name, std::string_view version) :
            file_name_(file_name.data(), file_name.size()), version_{version.data(), version.size()} {
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
            std::vector<std::string> prefix_list = {"", "lib"};
            std::vector<std::string> suffix_list = platform_suffixes(version_);
            suffix_list.emplace_back();

            for (const auto &prefix: prefix_list) {
                for (const auto &suffix: suffix_list) {
                    const std::string_view current{file_name_};
                    if (!prefix.empty() && current.find(prefix) == 0) {
                        continue;
                    }
                    if (!suffix.empty() && current.size() >= suffix.size() &&
                        current.compare(current.size() - suffix.size(), suffix.size(), suffix) == 0) {
                        continue;
                    }

                    std::string candidate = prefix + file_name_ + suffix;
                    std::string attempt = candidate;

                    if (!is_absolute_path(candidate)) {
                        std::error_code ec;
                        auto abs = std::filesystem::absolute(candidate, ec);
                        if (!ec) {
                            attempt = abs.string();
                        }
                    }

                    handle_ = ::LoadLibraryA(attempt.c_str());
                    if (handle_) {
                        qualified_file_name_ = attempt;
                        break;
                    }
                }
                if (handle_) {
                    break;
                }
            }

            if (!handle_) {
                error_ = "Cannot load library '" + file_name_ + "': " + win_error();
                return false;
            }

            error_.clear();
            return true;
        }

        bool unload_native() {
            if (!::FreeLibrary((HMODULE) handle_)) {
                error_ = "Cannot unload library '" + file_name_ + "': " + win_error();
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
