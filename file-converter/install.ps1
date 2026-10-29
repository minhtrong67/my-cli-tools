# Cai dat Converter Tool vao context menu (chuot phai) cua Windows. Khong can quyen admin (HKCU).
$ErrorActionPreference = 'Stop'

$installDir = Join-Path $env:LOCALAPPDATA 'ConverterTool'
New-Item -ItemType Directory -Force -Path $installDir | Out-Null
Copy-Item (Join-Path $PSScriptRoot 'convert.ps1') $installDir -Force
if (Test-Path (Join-Path $PSScriptRoot 'ffmpeg.exe')) {
    Copy-Item (Join-Path $PSScriptRoot 'ffmpeg.exe') $installDir -Force
}

# --- ffmpeg ---
$hasFfmpeg = (Test-Path (Join-Path $installDir 'ffmpeg.exe')) -or
             [bool](Get-Command ffmpeg.exe -ErrorAction SilentlyContinue) -or
             [bool](Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages" -Recurse -Filter ffmpeg.exe -ErrorAction SilentlyContinue | Select-Object -First 1)

if (-not $hasFfmpeg) {
    Write-Host 'Chua co ffmpeg (can cho video -> GIF, WebP, ICO).' -ForegroundColor Yellow
    if (Get-Command winget -ErrorAction SilentlyContinue) {
        $ans = Read-Host 'Cai ffmpeg bang winget ngay bay gio? (Y/n)'
        if ($ans -eq '' -or $ans -match '^[yY]') {
            winget install --id Gyan.FFmpeg -e --accept-source-agreements --accept-package-agreements
        }
    }
    else {
        Write-Host 'Khong co winget. Tai ffmpeg.exe va dat canh convert.ps1 trong: ' $installDir
    }
}

# --- Registry ---
$script = Join-Path $installDir 'convert.ps1'
function New-Cmd([string]$action) {
    return "powershell.exe -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File `"$script`" -Action $action -Path `"%1`""
}

$imageExts = '.png', '.jpg', '.jpeg', '.webp', '.bmp', '.gif', '.tif', '.tiff', '.ico'
$videoExts = '.mp4', '.mov', '.mkv', '.avi', '.webm', '.wmv', '.flv', '.m4v'

$imageItems = [ordered]@{
    '01_png'  = @('Convert to PNG', 'png')
    '02_jpg'  = @('Convert to JPG', 'jpg')
    '03_webp' = @('Convert to WebP', 'webp')
    '04_bmp'  = @('Convert to BMP', 'bmp')
    '05_gif'  = @('Convert to GIF', 'gif')
    '06_tiff' = @('Convert to TIFF', 'tiff')
    '07_ico'  = @('Convert to ICO', 'ico')
}
$videoItems = [ordered]@{
    '01_small' = @('Video to GIF (small, 480px, 12fps)', 'gif-small')
    '02_hq'    = @('Video to GIF (high quality, 720px, 15fps)', 'gif-hq')
}

function Add-Menu([string]$ext, $items) {
    $root = "HKCU:\Software\Classes\SystemFileAssociations\$ext\shell\ConverterTool"
    New-Item -Path $root -Force | Out-Null
    New-ItemProperty -Path $root -Name 'MUIVerb' -Value 'Converter Tool' -Force | Out-Null
    New-ItemProperty -Path $root -Name 'SubCommands' -Value '' -Force | Out-Null
    New-ItemProperty -Path $root -Name 'Icon' -Value 'imageres.dll,-5302' -Force | Out-Null
    New-Item -Path "$root\shell" -Force | Out-Null
    foreach ($key in $items.Keys) {
        $label, $action = $items[$key]
        $p = "$root\shell\$key"
        New-Item -Path $p -Force | Out-Null
        New-ItemProperty -Path $p -Name 'MUIVerb' -Value $label -Force | Out-Null
        New-Item -Path "$p\command" -Value (New-Cmd $action) -Force | Out-Null
    }
}

foreach ($e in $imageExts) { Add-Menu $e $imageItems }
foreach ($e in $videoExts) { Add-Menu $e $videoItems }

Write-Host ''
Write-Host 'Cai dat xong! Chuot phai len file anh/video -> Converter Tool.' -ForegroundColor Green
Write-Host '(Windows 11: bam "Show more options" hoac Shift+F10 de thay menu.)'
