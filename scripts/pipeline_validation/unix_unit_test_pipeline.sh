#!/usr/bin/env sh
#                                 Apache License
#                           Version 2.0, January 2004
#                        http://www.apache.org/licenses/
#
#   Copyright 2026 rainy-juzixiao
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

# =============================================================================
# ReTTR — Unix environment validation script
# =============================================================================
#
# Configures, builds and runs unit tests for both Debug and Release in a
# single invocation.  Use this to quickly verify that your development
# environment (compiler, CMake, system libraries) can build ReTTR correctly.
#
#   Usage:
#     ./validate_unix_environment.sh                # Debug + Release
#     ./validate_unix_environment.sh --debug-only    # Debug only
#     ./validate_unix_environment.sh --release-only  # Release only
#     ./validate_unix_environment.sh -j 8           # custom parallelism
#     ./validate_unix_environment.sh --generator "Unix Makefiles"
#     ./validate_unix_environment.sh --help
#
#   Options:
#     --debug-only            Build and test only the Debug configuration
#     --release-only          Build and test only the Release configuration
#     --generator <name>      CMake generator (default: Ninja if available,
#                             otherwise "Unix Makefiles")
#     --build-dir <path>      Base build directory (default: build)
#     -j <N>                  Number of parallel jobs (default: auto-detect)
#     --help, -h              Show this help message
# =============================================================================

set -eu

DEBUG_ONLY=""
RELEASE_ONLY=""
GENERATOR=""
JOBS=""
BUILD_DIR="build"
PASSED=0
FAILED=0

usage() {
    sed -n '/^# =/,/^# =/p' "$0" | sed '1d;$d;s/^# \?//'
    exit 0
}

die() {
    printf "ERROR: %s\n" "$1" >&2
    exit 1
}

phase() {
    printf "\n==> %s\n" "$1"
}

green()  { printf "\033[32m%s\033[0m\n" "$1"; }
red()    { printf "\033[31m%s\033[0m\n" "$1"; }

step_ok() {
    green "  [OK] $1"
    PASSED=$((PASSED + 1))
}

step_fail() {
    red "  [FAIL] $1"
    FAILED=$((FAILED + 1))
}

while [ $# -gt 0 ]; do
    case "$1" in
        --debug-only)
            DEBUG_ONLY="1"; shift
            ;;
        --release-only)
            RELEASE_ONLY="1"; shift
            ;;
        --generator)
            [ $# -ge 2 ] || die "--generator requires a value"
            GENERATOR="$2"; shift 2
            ;;
        --build-dir)
            [ $# -ge 2 ] || die "--build-dir requires a path argument"
            BUILD_DIR="$2"; shift 2
            ;;
        -j)
            [ $# -ge 2 ] || die "-j requires a number"
            JOBS="$2"; shift 2
            ;;
        --help|-h)
            usage
            ;;
        *)
            die "Unknown option: $1  (use --help)"
            ;;
    esac
done

if [ -z "${GENERATOR}" ]; then
    if command -v ninja >/dev/null 2>&1; then
        GENERATOR="Ninja"
    else
        GENERATOR="Unix Makefiles"
    fi
fi

if [ -z "${JOBS}" ]; then
    if command -v nproc >/dev/null 2>&1; then
        JOBS="$(nproc)"
    elif sysctl -n hw.ncpu >/dev/null 2>&1; then
        JOBS="$(sysctl -n hw.ncpu)"
    else
        JOBS="2"
    fi
fi

PROJECT_ROOT="$(CDPATH='' cd -- "$(dirname -- "$0")/../.." && pwd)"
[ -f "${PROJECT_ROOT}/CMakeLists.txt" ] || die "CMakeLists.txt not found at ${PROJECT_ROOT} — is the script inside the ReTTR source tree?"

command -v cmake >/dev/null 2>&1 || die "cmake is required but not found in PATH."
command -v ctest >/dev/null 2>&1 || die "ctest is required but not found in PATH."

# Detect OS
OS_NAME="$(uname -s)"
case "${OS_NAME}" in
    Linux)   PLATFORM="linux" ;;
    Darwin)  PLATFORM="macos" ;;
    *)
        die "Unsupported platform: ${OS_NAME}.  This script supports Linux and macOS."
        ;;
esac

phase "ReTTR environment validation — ${PLATFORM}"
printf "  Debug        : %s\n" "$([ -n "${RELEASE_ONLY}" ] && echo "skip" || echo "yes")"
printf "  Release      : %s\n" "$([ -n "${DEBUG_ONLY}" ]   && echo "skip" || echo "yes")"
printf "  Generator    : %s\n" "${GENERATOR}"
printf "  Build dir    : %s\n" "${BUILD_DIR}"
printf "  Jobs         : %s\n" "${JOBS}"
printf "  Project root : %s\n" "${PROJECT_ROOT}"

printf "\nToolchain:\n"
printf "  cmake : %s\n" "$(cmake --version | head -1)"
printf "  g++   : %s\n" "$(g++ --version 2>/dev/null | head -1 || echo "not found")"

if [ -z "${RELEASE_ONLY}" ]; then
    DEBUG_DIR="${BUILD_DIR}/debug"

    phase "Configure (Debug)"
    cmake -S "${PROJECT_ROOT}" -B "${DEBUG_DIR}" \
        -DCMAKE_BUILD_TYPE=Debug \
        -G "${GENERATOR}" \
        -DRETTR_BUILD_EXAMPLES=ON \
        -DRETTR_BUILD_UNIT_TESTS=ON \
        -DRETTR_USE_CXX26_REFLECTION_TS=OFF
    step_ok "CMake configured (Debug)"

    phase "Build (Debug)"
    cmake --build "${DEBUG_DIR}" -j "${JOBS}"
    step_ok "Build finished (Debug)"

    phase "Test (Debug)"
    if ctest --test-dir "${DEBUG_DIR}" --output-on-failure; then
        step_ok "All tests passed (Debug)"
    else
        step_fail "One or more tests FAILED (Debug)"
    fi
fi

if [ -z "${DEBUG_ONLY}" ]; then
    RELEASE_DIR="${BUILD_DIR}/release"

    phase "Configure (Release)"
    cmake -S "${PROJECT_ROOT}" -B "${RELEASE_DIR}" \
        -DCMAKE_BUILD_TYPE=Release \
        -G "${GENERATOR}" \
        -DRETTR_BUILD_EXAMPLES=ON \
        -DRETTR_BUILD_UNIT_TESTS=ON \
        -DRETTR_USE_CXX26_REFLECTION_TS=OFF
    step_ok "CMake configured (Release)"

    phase "Build (Release)"
    cmake --build "${RELEASE_DIR}" -j "${JOBS}"
    step_ok "Build finished (Release)"

    phase "Test (Release)"
    if ctest --test-dir "${RELEASE_DIR}" --output-on-failure; then
        step_ok "All tests passed (Release)"
    else
        step_fail "One or more tests FAILED (Release)"
    fi
fi

TOTAL=$((PASSED + FAILED))
phase "Validation result"
printf "  passed: %d  failed: %d  total: %d\n" "${PASSED}" "${FAILED}" "${TOTAL}"

if [ "${FAILED}" -eq 0 ]; then
    green "All validation steps passed."
else
    red "Validation completed with ${FAILED} failure(s) — check the output above."
    exit 1
fi
