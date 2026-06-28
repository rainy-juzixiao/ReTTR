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
# ReTTR — Windows uninstall script (PowerShell)
# =============================================================================
#
# Removes a previous installation of ReTTR.
#
# Two strategies, tried in order:
#   1) CMake install manifest — if <build-dir>\install_manifest.txt still
#      exists we simply remove every file listed in it.
#   2) Directory removal — otherwise we remove the well-known installation
#      subtrees underneath the prefix.
#
#   Usage:
#     .\uninstall.ps1                              # default prefix
#     .\uninstall.ps1 --prefix C:\tools\ReTTR       # Unix-style
#     .\uninstall.ps1 -Prefix C:\tools\ReTTR        # PowerShell-style
#     .\uninstall.ps1 --build-dir build_debug        # custom build dir
#     .\uninstall.ps1 --dry-run                      # preview without deleting
#     .\uninstall.ps1 --skip-test                    # ignored (build-time)
#
#   Parameters:
#     -Prefix / --prefix <path>         Installation prefix (default: <build-dir>\install)
#     -BuildDir / --build-dir           CMake build directory containing install_manifest.txt
#     -SkipTest / --skip-test           Ignored — build-time option kept for script uniformity
#     -SkipExamples / --skip-examples   Ignored — build-time option kept for script uniformity
#     -NoReflectionTs / --no-reflection-ts  Ignored — build-time option kept for script uniformity
#     -WhatIf / --dry-run               Print what would be removed, do not delete
#     -Help / --help / -h               Show this help message
# =============================================================================

# --- argument parsing ---
$Prefix = ""
$BuildDir = "build"
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
        '^-?-[Bb]uild[Dd]ir$|^--build-dir$' {
            $i++
            if ($i -ge $args.Count) { Write-Host "ERROR: --build-dir requires a path argument" -ForegroundColor Red; exit 1 }
            $BuildDir = $args[$i]
        }
        '^-?-[Ss]kip[Tt]est$|^--skip-test$' {
            # build-time option — ignored during uninstall
        }
        '^-?-[Ss]kip[Ee]xamples$|^--skip-examples$' {
            # build-time option — ignored during uninstall
        }
        '^-?-[Nn]o[Rr]eflection[Tt]s$|^--no-reflection-ts$' {
            # build-time option — ignored during uninstall
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

$ScriptDir = Split-Path -Parent $PSCommandPath
$ProjectRoot = Split-Path -Parent $ScriptDir

if (-not (Test-Path $ProjectRoot/CMakeLists.txt)) {
    Die "CMakeLists.txt not found at $ProjectRoot — is the script inside the ReTTR source tree?"
}

if ([string]::IsNullOrEmpty($Prefix)) {
    $Prefix = "$ProjectRoot/$BuildDir/install"
}
$Prefix = [System.IO.Path]::GetFullPath($Prefix)
$Manifest = "$ProjectRoot/$BuildDir/install_manifest.txt"

Phase "ReTTR uninstall — Windows"
Write-Host "  Prefix    : $Prefix"
Write-Host "  Build dir : $BuildDir"
if ($WhatIf) { Write-Host "  Mode      : dry-run (no files will be removed)" }

function Remove-File {
    param([string]$Path)
    if ($WhatIf) {
        Write-Host "  would remove: $Path"
        return
    }
    if (Test-Path -LiteralPath $Path -PathType Leaf) {
        Remove-Item -LiteralPath $Path -Force -ErrorAction SilentlyContinue
        Write-Host "  removed: $Path"
    }
}

function Remove-Tree {
    param([string]$Path)
    if ($WhatIf) {
        Write-Host "  would remove tree: $Path"
        return
    }
    if (Test-Path -LiteralPath $Path -PathType Container) {
        Remove-Item -LiteralPath $Path -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "  removed tree: $Path"
    }
}

# --- Strategy 1: install manifest ---
if (Test-Path -LiteralPath $Manifest) {
    Phase "Using install manifest: $Manifest"
    $count = 0
    Get-Content -LiteralPath $Manifest | ForEach-Object {
        if (-not [string]::IsNullOrEmpty($_)) {
            Remove-File $_
            $count++
        }
    }
    Write-Host "  $count entries processed from manifest."

    # Clean up any lingering DLLs / libs
    Get-ChildItem "$Prefix/lib/*rettr*package*" -ErrorAction SilentlyContinue | ForEach-Object {
        Remove-File $_.FullName
    }

    if (-not $WhatIf) {
        Remove-Tree "$Prefix/include/rettr"
        Remove-Tree "$Prefix/lib/cmake/ReTTR"
        Remove-Item "$Prefix/share/rettr/version.txt" -Force -ErrorAction SilentlyContinue
    }

    Write-Host "`nUninstall complete."
    exit 0
}

# --- Strategy 2: known subtrees ---
Phase "No install manifest found — removing known installation subtrees"
Remove-Tree "$Prefix/include/rettr"

Get-ChildItem "$Prefix/lib/librettr*" -ErrorAction SilentlyContinue | ForEach-Object {
    Remove-File $_.FullName
}
Get-ChildItem "$Prefix/lib/rettr*" -ErrorAction SilentlyContinue | ForEach-Object {
    Remove-File $_.FullName
}
Get-ChildItem "$Prefix/bin/rettr*" -ErrorAction SilentlyContinue | ForEach-Object {
    Remove-File $_.FullName
}

Remove-Tree "$Prefix/lib/cmake/ReTTR"

if (-not $WhatIf) {
    # Clean up empty parent dirs if they're now empty
    $null = Remove-Item "$Prefix/share/rettr/version.txt" -Force -ErrorAction SilentlyContinue
    $null = Remove-Item "$Prefix/share/rettr" -Force -ErrorAction SilentlyContinue 2>$null
}

Write-Host "`nUninstall complete."
