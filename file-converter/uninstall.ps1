# Go Converter Tool khoi context menu
$exts = '.png', '.jpg', '.jpeg', '.webp', '.bmp', '.gif', '.tif', '.tiff', '.ico',
        '.mp4', '.mov', '.mkv', '.avi', '.webm', '.wmv', '.flv', '.m4v'
foreach ($e in $exts) {
    $p = "HKCU:\Software\Classes\SystemFileAssociations\$e\shell\ConverterTool"
    if (Test-Path $p) { Remove-Item $p -Recurse -Force }
}
$dir = Join-Path $env:LOCALAPPDATA 'ConverterTool'
if (Test-Path $dir) { Remove-Item $dir -Recurse -Force -ErrorAction SilentlyContinue }
Write-Host 'Da go cai dat Converter Tool.' -ForegroundColor Green
