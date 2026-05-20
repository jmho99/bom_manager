$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$PackageFile = Join-Path $ScriptDir "winget-packages.txt"

Get-Content $PackageFile | ForEach-Object {
    $Package = $_.Trim()

    if ($Package -eq "" -or $Package.StartsWith("#")) {
        return
    }

    winget install --id $Package --exact --accept-source-agreements --accept-package-agreements
}

Write-Host ""
Write-Host "Basic Windows build dependencies installed."
Write-Host "Install Qt 6 separately using Qt Online Installer or Qt Maintenance Tool."
