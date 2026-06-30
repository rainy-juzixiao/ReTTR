#!/usr/bin/env python3
"""
Usage:
    python fetch_cmake_import_std_uuid.py
    python fetch_cmake_import_std_uuid.py --version 3.30.0   # specify manually
    python fetch_cmake_import_std_uuid.py --quiet             # only print the UUID (for scripting)
"""
import argparse
import re
import subprocess
import sys
import urllib.request
import urllib.error

RAW_URL_TEMPLATE = "https://raw.githubusercontent.com/Kitware/CMake/v{version}/Help/dev/experimental.rst"
MASTER_URL = "https://raw.githubusercontent.com/Kitware/CMake/master/Help/dev/experimental.rst"
UUID_RE = re.compile(
    r"CMAKE_EXPERIMENTAL_CXX_IMPORT_STD.*?"
    r"([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12})",
    re.DOTALL,
)


def detect_local_cmake_version() -> str:
    """Detect the installed local CMake version via `cmake --version`."""
    try:
        out = subprocess.check_output(["cmake", "--version"], text=True)
    except (FileNotFoundError, subprocess.CalledProcessError):
        return None
    m = re.search(r"cmake version (\d+\.\d+\.\d+)", out)
    return m.group(1) if m else None


def fetch_text(url: str) -> str:
    req = urllib.request.Request(url, headers={"User-Agent": "cmake-import-std-uuid-fetcher"})
    with urllib.request.urlopen(req, timeout=10) as resp:
        return resp.read().decode("utf-8")
    return None


def fetch_experimental_rst(version: str, quiet: bool = False) -> tuple[str, str]:
    """
    First try the experimental.rst from the matching version tag.
    If that file doesn't exist for this version (too old, feature not present yet)
    or the tag doesn't exist, fall back to the master branch
    (for reference only — verify applicability yourself).
    Returns (content, actual URL used).
    """
    tag_url = RAW_URL_TEMPLATE.format(version=version)
    try:
        return fetch_text(tag_url), tag_url
    except urllib.error.HTTPError as e:
        if e.code != 404:
            raise
    except urllib.error.URLError:
        pass
    if not quiet:
        print(f"[warning] Could not find experimental.rst for v{version}, falling back to master branch",
              file=sys.stderr)
    return fetch_text(MASTER_URL), MASTER_URL


def extract_import_std_uuid(rst_text: str) -> str:
    m = UUID_RE.search(rst_text)
    return m.group(1) if m else None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version",
                        help="Manually specify a CMake version, e.g. 3.30.0; auto-detects local version if omitted")
    parser.add_argument("-q", "--quiet", action="store_true",
                        help="Only print the bare UUID to stdout (for use in scripts/CI pipelines)")
    args = parser.parse_args()

    version = args.version or detect_local_cmake_version()
    if not version:
        print("Could not detect local CMake version. Use --version to specify one manually (e.g. 3.30.0)",
              file=sys.stderr)
        sys.exit(1)

    if not args.quiet:
        print(f"Detected CMake version: {version}")

    try:
        rst_text, used_url = fetch_experimental_rst(version, quiet=args.quiet)
        if not args.quiet:
            print(f"Fetched documentation from {used_url}")

        uuid = extract_import_std_uuid(rst_text)
        if uuid:
            if args.quiet:
                print(uuid)
            else:
                print(f"\nUUID for CMAKE_EXPERIMENTAL_CXX_IMPORT_STD: {uuid}")
                print(
                    "\nUse it like this in CMakeLists.txt, before project() (for reference only — nothing is written automatically):")
                print(f'  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "{uuid}")')
        else:
            print("\nCould not find an import std UUID in the document.", file=sys.stderr)
            print(
                "This CMake version may not support this experimental feature yet, or the doc format changed. Check manually:",
                file=sys.stderr)
            print(f"  {used_url}", file=sys.stderr)
            sys.exit(2)
    except Exception as e:
        print(f"Failed to fetch documentation: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
