$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path "$ScriptDir\.."
$BuildDir = Join-Path $ProjectRoot "build"

cmake -S $ProjectRoot -B $BuildDir -G Ninja
cmake --build $BuildDir

Write-Host ""
Write-Host "Build complete."
Write-Host "Executable path: $BuildDir\inventory_bom_manager.exe"
