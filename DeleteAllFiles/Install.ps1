$ErrorActionPreference = 'Stop'
$src  = $PSScriptRoot
$dest = Join-Path $env:LOCALAPPDATA 'DeleteAllFiles'

New-Item -ItemType Directory -Force -Path $dest | Out-Null
foreach ($f in 'DeleteAllFiles.ps1', 'run.vbs') { Copy-Item -LiteralPath (Join-Path $src $f) -Destination $dest -Force }
if (-not (Test-Path (Join-Path $dest 'config.json'))) { Copy-Item (Join-Path $src 'config.json') $dest -Force }

$icoDir = Join-Path $dest 'icons'
New-Item -ItemType Directory -Force -Path $icoDir | Out-Null
Copy-Item -Path (Join-Path $src 'icons\*.ico') -Destination $icoDir -Force

$vbs = Join-Path $dest 'run.vbs'

function Add-Menu($base) {
    New-Item -Path $base -Force | Out-Null
    Set-ItemProperty -Path $base -Name 'MUIVerb'  -Value 'Delete all files'
    Set-ItemProperty -Path $base -Name 'Icon'     -Value (Join-Path $icoDir 'main.ico')
    Set-ItemProperty -Path $base -Name 'SubCommands' -Value ''

    $items = @(
        @{ Key = '1recycle';   Label = 'Move to Recycle Bin (default)'; Mode = 'Recycle';   Icon = (Join-Path $icoDir 'recycle.ico') },
        @{ Key = '2permanent'; Label = 'Delete permanently';            Mode = 'Permanent'; Icon = (Join-Path $icoDir 'permanent.ico') }
    )
    foreach ($i in $items) {
        $k = "$base\shell\$($i.Key)"
        New-Item -Path "$k\command" -Force | Out-Null
        Set-ItemProperty -Path $k -Name 'MUIVerb' -Value $i.Label
        Set-ItemProperty -Path $k -Name 'Icon'    -Value $i.Icon
        Set-ItemProperty -Path "$k\command" -Name '(default)' -Value "wscript.exe `"$vbs`" `"%V`" $($i.Mode)"
    }
}

Add-Menu 'HKCU:\Software\Classes\Directory\shell\DeleteAllFiles'
Add-Menu 'HKCU:\Software\Classes\Directory\Background\shell\DeleteAllFiles'

Write-Host 'Installed. Right-click a folder (or empty space inside a folder) > Delete all files.'
Write-Host 'Windows 11: click "Show more options" (or Shift+F10) to see the entry.'
