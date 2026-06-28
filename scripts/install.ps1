<#
                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   Copyright 2026 rainy-juzixiao

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
#>

# =============================================================================
# ReTTR — Windows install script (PowerShell)
# =============================================================================
#
# Builds ReTTR with CMake and installs it under <build-dir>/install.
#
#   Usage:
#     .\install.ps1                             # Release build, default prefix
#     .\install.ps1 -Prefix C:\tools\ReTTR       # PowerShell-style
#     .\install.ps1 --prefix C:\tools\ReTTR      # Unix-style
#     .\install.ps1 --static                     # build static library
#     .\install.ps1 --debug                      # Debug build
#     .\install.ps1 --skip-test --skip-examples  # skip tests and examples
#     .\install.ps1 --no-reflection-ts           # disable C++26 Reflection TS support
#     .\install.ps1 --dry-run                    # preview without building
#
#   Parameters:
#     -Prefix / --prefix <path>   Installation prefix (default: <build-dir>\install)
#     -Static / --static          Build a static library instead of the default shared
#     -Debug / --debug            Build with debug symbols
#     -BuildDir / --build-dir     CMake build directory (default: build)
#     -Jobs / -j <N>              Number of parallel jobs (default: auto-detect)
#     -SkipTest / --skip-test     Skip building unit tests
#     -SkipExamples / --skip-examples  Skip building example programs
#     -NoReflectionTs / --no-reflection-ts  Disable C++26 Reflection TS support (enabled by default)
#     -WhatIf / --dry-run         Print what would be done without executing
#     -Help / --help / -h         Show this help message
# =============================================================================

# --- argument parsing ---
$Prefix = ""
$Static = $false
$Debug = $false
$BuildDir = "build"
$Jobs = 0
$WhatIf = $false
$Help = $false
$SkipTest = $false
$SkipExamples = $false
$NoReflectionTs = $false

$i = 0
while ($i -lt $args.Count) {
    $a = $args[$i]
    switch -Regex ($a) {
        '^-?-[Pp]refix$|^--prefix$' {
            $i++
            if ($i -ge $args.Count) { Write-Host "ERROR: --prefix requires a path argument" -ForegroundColor Red; exit 1 }
            $Prefix = $args[$i]
        }
        '^-?-[Ss]tatic$|^--static$' {
            $Static = $true
        }
        '^-?-[Dd]ebug$|^--debug$' {
            $Debug = $true
        }
        '^-?-[Bb]uild[Dd]ir$|^--build-dir$' {
            $i++
            if ($i -ge $args.Count) { Write-Host "ERROR: --build-dir requires a path argument" -ForegroundColor Red; exit 1 }
            $BuildDir = $args[$i]
        }
        '^-?-[Jj]obs$|^-j$|^--jobs$' {
            $i++
            if ($i -ge $args.Count) { Write-Host "ERROR: -j requires a number" -ForegroundColor Red; exit 1 }
            $Jobs = [int]$args[$i]
        }
        '^-?-[Ss]kip[Tt]est$|^--skip-test$' {
            $SkipTest = $true
        }
        '^-?-[Ss]kip[Ee]xamples$|^--skip-examples$' {
            $SkipExamples = $true
        }
        '^-?-[Nn]o[Rr]eflection[Tt]s$|^--no-reflection-ts$' {
            $NoReflectionTs = $true
        }
        '^-?-[Ww]hat[Ii]f$|^--dry-run$' {
            $WhatIf = $true
        }
        '^-?-[Hh](elp)?$|^--help$|^-h$' {
            $Help = $true
        }
        default {
            Write-Host "Unknown option: $a  (use --help)" -ForegroundColor Red
            exit 1
        }
    }
    $i++
}

function Show-Help {
    Get-Content -LiteralPath "$PSCommandPath" | Select-String '^#' | ForEach-Object { $_.ToString().Substring(2) }
    exit 0
}

function Die {
    param([string]$Message)
    Write-Host "ERROR: $Message" -ForegroundColor Red
    exit 1
}

function Phase {
    param([string]$Message)
    Write-Host "`n==> $Message" -ForegroundColor Green
}

if ($Help) { Show-Help }

# Resolve project root (scripts/ is one level below the repo root).
$ScriptDir = Split-Path -Parent $PSCommandPath
$ProjectRoot = Split-Path -Parent $ScriptDir
if (-not (Test-Path "$ProjectRoot/CMakeLists.txt")) {
    Die "CMakeLists.txt not found at $ProjectRoot — is the script inside the ReTTR source tree?"
}

$Config = if ($Debug) { "Debug" } else { "Release" }
$SharedLibs = if ($Static) { "OFF" } else { "ON" }

if ($Jobs -eq 0) {
    $Jobs = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
    if ($Jobs -le 0) { $Jobs = 4 }
}

# Default prefix: when not specified, install into <build-dir>/install
if ([string]::IsNullOrEmpty($Prefix)) {
    $Prefix = "$ProjectRoot/$BuildDir/install"
}

# Normalize to absolute path
$Prefix = [System.IO.Path]::GetFullPath($Prefix)

Phase "ReTTR $Config install — Windows"
Write-Host "  Prefix      : $Prefix"
Write-Host "  Shared libs : $SharedLibs"
Write-Host "  Build dir   : $BuildDir"
Write-Host "  Jobs        : $Jobs"
Write-Host "  Skip test   : $SkipTest"
Write-Host "  Skip exmpls : $SkipExamples"
Write-Host "  No refl. TS : $NoReflectionTs"

# Build up extra CMake options from flags
$CmakeExtraOpts = @()
if ($SkipTest)       { $CmakeExtraOpts += "-DRETTR_BUILD_UNIT_TESTS=OFF" }
if ($SkipExamples)   { $CmakeExtraOpts += "-DRETTR_BUILD_EXAMPLES=OFF" }
if ($NoReflectionTs) { $CmakeExtraOpts += "-DRETTR_USE_CXX26_REFLECTION_TS=OFF" }

if ($WhatIf) {
    Write-Host "`nDry-run — would run:"
    $OptStr = if ($CmakeExtraOpts.Count -gt 0) { " $($CmakeExtraOpts -join ' ')" } else { "" }
    Write-Host "  cmake -S $ProjectRoot -B $BuildDir -DCMAKE_BUILD_TYPE=$Config -DCMAKE_INSTALL_PREFIX=$Prefix -DRETTR_BUILD_WITH_DYNAMIC=$SharedLibs$OptStr"
    Write-Host "  cmake --build $BuildDir --config $Config --parallel $Jobs"
    Write-Host "  cmake --install $BuildDir --config $Config"
    exit 0
}

Phase "Configure"
cmake -S "$ProjectRoot" -B "$BuildDir" `
    -DCMAKE_BUILD_TYPE="$Config" `
    -DCMAKE_INSTALL_PREFIX="$Prefix" `
    -DRETTR_BUILD_WITH_DYNAMIC="$SharedLibs" `
    @CmakeExtraOpts
if ($LASTEXITCODE -ne 0) { Die "CMake configuration failed." }

Phase "Build"
cmake --build "$BuildDir" --config "$Config" --parallel $Jobs
if ($LASTEXITCODE -ne 0) { Die "Build failed." }

Phase "Install"
cmake --install "$BuildDir" --config "$Config"
if ($LASTEXITCODE -ne 0) { Die "Install step failed." }

Phase "Verify"
$Header = "$Prefix/include/rettr/rettr.hpp"
if (Test-Path $Header) {
    Write-Host "  [OK] Headers installed: $Header"
} else {
    Write-Host "  [WARN] Main header not found at $Header"
}

# Check for any installed library file
$LibCount = (Get-ChildItem "$Prefix/lib/*rettr*package*" -ErrorAction SilentlyContinue).Count
Write-Host "  Found $LibCount librar`(ies`) in $Prefix/lib/"

# Check for DLL in both lib/ and bin/
$DllCount = (Get-ChildItem "$Prefix/lib/*.dll" -ErrorAction SilentlyContinue).Count +
            (Get-ChildItem "$Prefix/bin/*.dll" -ErrorAction SilentlyContinue).Count
if ($DllCount -gt 0) {
    Write-Host "  Found $DllCount DLL`(s`)"
}

$VersionFile = "$ProjectRoot/version.txt"
$Version = if (Test-Path $VersionFile) { Get-Content $VersionFile -Raw } else { "?" }

Write-Host "`nReTTR $Version installed successfully to $Prefix" -ForegroundColor Green
Write-Host "Use in CMake projects:"
Write-Host "  find_package(ReTTR REQUIRED)"
Write-Host "  target_link_libraries(<target> RETTR::rettr)"
