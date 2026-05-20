$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path "$ScriptDir\.."
$BuildDir = Join-Path $ProjectRoot "build"
$SourceExePath = Join-Path $BuildDir "inventory_bom_manager.exe"
$ReleaseDir = Join-Path $ProjectRoot "release\windows"
$ReleaseExePath = Join-Path $ReleaseDir "inventory_bom_manager.exe"

if (!(Test-Path $SourceExePath)) {
    throw "Executable not found: $SourceExePath"
}

if (Test-Path $ReleaseDir) {
    Remove-Item $ReleaseDir -Recurse -Force
}
New-Item -ItemType Directory -Path $ReleaseDir | Out-Null

Copy-Item $SourceExePath $ReleaseExePath
windeployqt $ReleaseExePath

# Release package must not include local test data.
Get-ChildItem -Path $ReleaseDir -Recurse -File -Include `
    "inventory_bom.db", "inventory_bom.db-*", "*.sqlite", "*.sqlite3", "*.jsonl" |
    Remove-Item -Force

$RuntimeDirs = @(
    (Join-Path $ReleaseDir "logs"),
    (Join-Path $ReleaseDir "exports")
)
foreach ($Dir in $RuntimeDirs) {
    if (Test-Path $Dir) {
        Remove-Item $Dir -Recurse -Force
    }
}

Write-Host ""
Write-Host "Windows release package created:"
Write-Host $ReleaseDir
