$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path "$ScriptDir\.."

Write-Host "Cleaning runtime data under project folder..."

$Patterns = @(
    "inventory_bom.db",
    "inventory_bom.db-*",
    "*.sqlite",
    "*.sqlite3",
    "*.jsonl"
)

foreach ($Pattern in $Patterns) {
    Get-ChildItem -Path $ProjectRoot -Recurse -File -Filter $Pattern -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -notmatch "\\.git\\" } |
        Remove-Item -Force
}

$Dirs = @(
    "$ProjectRoot\logs",
    "$ProjectRoot\exports",
    "$ProjectRoot\build\logs",
    "$ProjectRoot\build\exports",
    "$ProjectRoot\release\logs",
    "$ProjectRoot\release\exports"
)

foreach ($Dir in $Dirs) {
    if (Test-Path $Dir) {
        Remove-Item $Dir -Recurse -Force
    }
}

Write-Host "Runtime data cleanup complete."
