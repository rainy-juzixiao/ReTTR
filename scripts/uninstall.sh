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
# ReTTR — POSIX uninstall script (Linux / macOS)
# =============================================================================
#
# Removes a previous installation of ReTTR from a system-wide prefix.
#
# Two strategies, tried in order:
#   1) CMake install manifest — if <build-dir>/install_manifest.txt still
#      exists we simply remove every file listed in it.
#   2) Directory removal — otherwise we remove the well-known installation
#      subtrees underneath the prefix.
#
#   Usage:
#     ./uninstall.sh                          # default prefix: /usr/local
#     ./uninstall.sh --prefix /usr            # custom prefix
#     ./uninstall.sh --build-dir build        # specify build dir (for manifest)
#     ./uninstall.sh --dry-run                # preview without deleting
#     ./uninstall.sh --help
#
#   Options:
#     --prefix <path>    Installation prefix (default: /usr/local)
#     --build-dir <path> CMake build directory containing install_manifest.txt
#     --dry-run          Print what would be removed, do not delete
#     --help, -h         Show this help message
# =============================================================================

set -eu

# --- defaults ---------------------------------------------------------------
PREFIX="/usr/local"
BUILD_DIR="build"
DRY_RUN=""

# --- helpers ----------------------------------------------------------------
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

remove_file() {
    if [ -n "${DRY_RUN}" ]; then
        printf "  would remove: %s\n" "$1"
        return 0
    fi
    if [ -f "$1" ] || [ -L "$1" ]; then
        rm -f "$1" && printf "  removed: %s\n" "$1"
    elif [ -d "$1" ]; then
        # shellcheck disable=SC2015
        rmdir "$1" 2>/dev/null && printf "  removed dir: %s\n" "$1" || true
    fi
}

remove_tree() {
    if [ -n "${DRY_RUN}" ]; then
        printf "  would remove tree: %s\n" "$1"
        return 0
    fi
    if [ -d "$1" ]; then
        rm -rf "$1" && printf "  removed tree: %s\n" "$1"
    fi
}

while [ $# -gt 0 ]; do
    case "$1" in
        --prefix)
            [ $# -ge 2 ] || die "--prefix requires a path argument"
            PREFIX="$2"; shift 2
            ;;
        --build-dir)
            [ $# -ge 2 ] || die "--build-dir requires a path argument"
            BUILD_DIR="$2"; shift 2
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

# Project root is one level above the scripts/ directory.
PROJECT_ROOT="$(CDPATH='' cd -- "$(dirname -- "$0")/.." && pwd)"

[ -f "${PROJECT_ROOT}/CMakeLists.txt" ] || die "CMakeLists.txt not found at ${PROJECT_ROOT} — is the script inside the ReTTR source tree?"

PREFIX="${PREFIX%/}"

case "${PREFIX}" in
    /|/usr|/usr/local|/opt|/opt/homebrew) ;;
    *)
        ;;
esac

phase "ReTTR uninstall — ${PLATFORM}"
printf "  Prefix        : %s\n" "${PREFIX}"
printf "  Build dir     : %s\n" "${BUILD_DIR}"
if [ -n "${DRY_RUN}" ]; then
    printf "  Mode          : dry-run (no files will be removed)\n"
fi
MANIFEST="${BUILD_DIR}/install_manifest.txt"

if [ -f "${MANIFEST}" ]; then
    phase "Using install manifest: ${MANIFEST}"

    count=0
    while IFS= read -r entry; do
        [ -z "${entry}" ] && continue
        remove_file "${entry}"
        count=$((count + 1))
    done < "${MANIFEST}"

    printf "\n  %d entries processed from manifest.\n" "${count}"

    for candidate in "${PREFIX}/lib"/*rettr-*-package*; do
        [ -e "${candidate}" ] || continue
        remove_file "${candidate}"
    done

    if [ -z "${DRY_RUN}" ]; then
        rm -rf "${PREFIX}/include/rettr" 2>/dev/null || true
        rm -rf "${PREFIX}/lib/cmake/ReTTR" 2>/dev/null || true
        rm -rf "${PREFIX}/share/rettr" 2>/dev/null || true
    fi

    printf "\nUninstall complete.\n"
    exit 0
fi

phase "No install manifest found — removing known installation subtrees"

HDR_COUNT=0
LIB_COUNT=0
CFG_COUNT=0

if [ -d "${PREFIX}/include/rettr" ]; then
    remove_tree "${PREFIX}/include/rettr"
    HDR_COUNT=1
fi

for candidate in "${PREFIX}/lib"/librettr*; do
    [ -e "${candidate}" ] || continue
    remove_file "${candidate}"
    LIB_COUNT=$((LIB_COUNT + 1))
done

# 3. CMake package files
if [ -d "${PREFIX}/lib/cmake/ReTTR" ]; then
    remove_tree "${PREFIX}/lib/cmake/ReTTR"
    CFG_COUNT=1
fi

# 4. Version file
if [ -f "${PREFIX}/share/rettr/version.txt" ]; then
    remove_file "${PREFIX}/share/rettr/version.txt"
fi

# shellcheck disable=SC2015
[ -d "${PREFIX}/share/rettr" ] && rmdir "${PREFIX}/share/rettr" 2>/dev/null || true
# shellcheck disable=SC2015
[ -d "${PREFIX}/share"       ] && rmdir "${PREFIX}/share"       2>/dev/null || true

if [ -z "${DRY_RUN}" ]; then
    rmdir "${PREFIX}/lib/cmake" 2>/dev/null || true
    rmdir "${PREFIX}/lib"       2>/dev/null || true
    rmdir "${PREFIX}/include"   2>/dev/null || true
fi

printf "\nUninstall complete — %d header tree(s), %d librar%s, %d CMake config(s).\n" \
    "${HDR_COUNT}" "${LIB_COUNT}" "$([ "${LIB_COUNT}" -eq 1 ] && printf "y" || printf "ies")" "${CFG_COUNT}"

if [ "${HDR_COUNT}" -eq 0 ] && [ "${LIB_COUNT}" -eq 0 ] && [ "${CFG_COUNT}" -eq 0 ]; then
    printf "Nothing was found to remove under %s\n" "${PREFIX}"
fi
