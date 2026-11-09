Remove-Item -Path 'HKCU:\Software\Classes\Directory\shell\DeleteAllFiles' -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path 'HKCU:\Software\Classes\Directory\Background\shell\DeleteAllFiles' -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path (Join-Path $env:LOCALAPPDATA 'DeleteAllFiles') -Recurse -Force -ErrorAction SilentlyContinue
Write-Host 'Uninstalled.'
