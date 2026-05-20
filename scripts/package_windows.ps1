$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path "$ScriptDir\.."
$BuildDir = Join-Path $ProjectRoot "build"
$ExePath = Join-Path $BuildDir "inventory_bom_manager.exe"

if (!(Test-Path $ExePath)) {
    throw "Executable not found: $ExePath"
}

windeployqt $ExePath

Write-Host ""
Write-Host "Windows deployment files collected by windeployqt."
