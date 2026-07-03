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
# ReTTR — POSIX install script (Linux / macOS)
# =============================================================================
#
# Builds ReTTR with CMake and installs it to a system-wide prefix.
#
#   Usage:
#     ./install.sh                          # default prefix: /usr/local
#     ./install.sh --prefix /usr            # custom prefix
#     ./install.sh --prefix /usr --static   # build and install static library
#     ./install.sh --skip-test --skip-examples  # skip tests and examples
#     ./install.sh --no-reflection-ts           # disable C++26 Reflection TS support
#     ./install.sh --help
#
#   Options:
#     --prefix <path>         Installation prefix (default: /usr/local)
#     --static                Build a static library instead of the default shared
#     --debug                 Build with debug symbols
#     --build-dir <path>      CMake build directory (default: build)
#     -j <N>                  Number of parallel jobs (default: auto-detect)
#     --skip-test             Skip building unit tests
#     --skip-examples         Skip building example programs
#     --no-reflection-ts      Disable C++26 Reflection TS support (enabled by default)
#     --help, -h              Show this help message
# =============================================================================

set -eu

PREFIX="/usr/local"
BUILD_TYPE="Release"
BUILD_DIR="build"
SHARED_LIBS="ON"
JOBS=""
DRY_RUN=""
SKIP_TESTS=""
SKIP_EXAMPLES=""
NO_REFLECTION_TS=""

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

while [ $# -gt 0 ]; do
    case "$1" in
        --prefix)
            [ $# -ge 2 ] || die "--prefix requires a path argument"
            PREFIX="$2"; shift 2
            ;;
        --static)
            SHARED_LIBS="OFF"; shift
            ;;
        --debug)
            BUILD_TYPE="Debug"; shift
            ;;
        --build-dir)
            [ $# -ge 2 ] || die "--build-dir requires a path argument"
            BUILD_DIR="$2"; shift 2
            ;;
        -j)
            [ $# -ge 2 ] || die "-j requires a number"
            JOBS="$2"; shift 2
            ;;
        --skip-test)
            SKIP_TESTS="1"; shift
            ;;
        --skip-examples)
            SKIP_EXAMPLES="1"; shift
            ;;
        --no-reflection-ts)
            NO_REFLECTION_TS="1"; shift
            ;;
        --dry-run)
            DRY_RUN="1"; shift
            ;;
        --help|-h)
            usage
            ;;
        *)
            die "Unknown option: $1  (use --help)"
            ;;
    esac
done

OS_NAME="$(uname -s)"
case "${OS_NAME}" in
    Linux)   PLATFORM="linux" ;;
    Darwin)  PLATFORM="macos" ;;
    *)
        die "Unsupported platform: ${OS_NAME}.  This script supports Linux and macOS."
        ;;
esac

if [ -z "${JOBS}" ]; then
    if command -v nproc >/dev/null 2>&1; then
        JOBS="$(nproc)"
    elif sysctl -n hw.ncpu >/dev/null 2>&1; then
        JOBS="$(sysctl -n hw.ncpu)"
    else
        JOBS="2"
    fi
fi

command -v cmake >/dev/null 2>&1 || die "cmake is required but not found in PATH."

phase "ReTTR ${BUILD_TYPE} install — ${PLATFORM}"
printf "  Prefix      : %s\n" "${PREFIX}"
printf "  Shared libs : %s\n" "${SHARED_LIBS}"
printf "  Build dir   : %s\n" "${BUILD_DIR}"
printf "  Jobs        : %s\n" "${JOBS}"
printf "  Skip tests  : %s\n" "${SKIP_TESTS:-no}"
printf "  Skip exmpls : %s\n" "${SKIP_EXAMPLES:-no}"
printf "  No refl. TS : %s\n" "${NO_REFLECTION_TS:-no}"

# Project root is one level above the scripts/ directory.
PROJECT_ROOT="$(CDPATH='' cd -- "$(dirname -- "$0")/.." && pwd)"

[ -f "${PROJECT_ROOT}/CMakeLists.txt" ] || die "CMakeLists.txt not found at ${PROJECT_ROOT} — is the script inside the ReTTR source tree?"

# Build up extra CMake options from flags
CMAKE_EXTRA_OPTS=""
[ -n "${SKIP_TESTS}" ]       && CMAKE_EXTRA_OPTS="${CMAKE_EXTRA_OPTS} -DRETTR_BUILD_UNIT_TESTS=OFF"
[ -n "${SKIP_EXAMPLES}" ]    && CMAKE_EXTRA_OPTS="${CMAKE_EXTRA_OPTS} -DRETTR_BUILD_EXAMPLES=OFF"
[ -n "${NO_REFLECTION_TS}" ] && CMAKE_EXTRA_OPTS="${CMAKE_EXTRA_OPTS} -DRETTR_USE_CXX26_REFLECTION_TS=OFF"

if [ -n "${DRY_RUN}" ]; then
    printf "\nDry-run — would run:\n"
    printf "  cmake -S %s -B %s -DCMAKE_BUILD_TYPE=%s -DCMAKE_INSTALL_PREFIX=%s -DRETTR_BUILD_WITH_DYNAMIC=%s%s\n" \
        "${PROJECT_ROOT}" "${BUILD_DIR}" "${BUILD_TYPE}" "${PREFIX}" "${SHARED_LIBS}" "${CMAKE_EXTRA_OPTS}"
    printf "  cmake --build %s --parallel %s\n" "${BUILD_DIR}" "${JOBS}"
    printf "  cmake --install %s\n" "${BUILD_DIR}"
    exit 0
fi

phase "Configure"
# shellcheck disable=SC2086
cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
    -DRETTR_BUILD_WITH_DYNAMIC="${SHARED_LIBS}" \
    ${CMAKE_EXTRA_OPTS}

phase "Build"
cmake --build "${BUILD_DIR}" --parallel "${JOBS}"

phase "Install"

# On macOS the install destination may require sudo; on Linux /usr/local
# is usually user-writable but we handle the failure gracefully.
if [ "${PREFIX}" = "/usr/local" ] || [ "${PREFIX}" = "/usr" ]; then
    if [ -w "${PREFIX}" ]; then
        cmake --install "${BUILD_DIR}"
    else
        printf "The prefix %s is not user-writable — elevating with sudo.\n" "${PREFIX}"
        sudo cmake --install "${BUILD_DIR}"
    fi
else
    cmake --install "${BUILD_DIR}"
fi

phase "Verify"

HEADER="${PREFIX}/include/rettr/rettr.hpp"
if [ -f "${HEADER}" ]; then
    printf "  [OK] Headers installed: %s\n" "${HEADER}"
else
    printf "  [WARN] Main header not found at %s — verify the install tree.\n" "${HEADER}"
fi

# Library name carries version info from the build system — use a glob.
LIB_COUNT=0
for candidate in "${PREFIX}/lib"/*rettr-*-package*; do
    [ -e "${candidate}" ] || continue
    printf "  [OK] Library installed: %s\n" "${candidate}"
    LIB_COUNT=$((LIB_COUNT + 1))
done
if [ "${LIB_COUNT}" -eq 0 ]; then
    printf "  [WARN] No library found at %s/lib/librettr*\n" "${PREFIX}"
fi

printf "\nReTTR %s installed successfully to %s\n" "$(cat "${PROJECT_ROOT}/version.txt" 2>/dev/null || echo "?")" "${PREFIX}"
printf "Use in CMake projects:\n"
printf "  find_package(ReTTR REQUIRED)\n"
printf "  target_link_libraries(<target> RETTR::rettr)\n"
