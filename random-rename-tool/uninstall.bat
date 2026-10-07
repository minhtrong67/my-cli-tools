@echo off
chcp 65001 >nul

reg delete "HKCU\Software\Classes\*\shell\RandomRenameQuick" /f >nul 2>nul
reg delete "HKCU\Software\Classes\Directory\shell\RandomRenameQuick" /f >nul 2>nul

REM remove entries from older versions, if present
reg delete "HKCU\Software\Classes\*\shell\RandomRenameCustom" /f >nul 2>nul
reg delete "HKCU\Software\Classes\Directory\shell\RandomRenameCustom" /f >nul 2>nul
reg delete "HKCU\Software\Classes\*\shell\RandomRename" /f >nul 2>nul
reg delete "HKCU\Software\Classes\Directory\shell\RandomRename" /f >nul 2>nul

echo.
echo Removed "Random Rename" from the right-click context menu.
echo (Your saved settings and the tool's files were not deleted.)
echo.
pause
