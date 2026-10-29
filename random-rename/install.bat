@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_PATH=%SCRIPT_DIR%random_rename.py"
set "ICON_QUICK=%SCRIPT_DIR%assets\rename_quick.ico"
set "PYW="

if not exist "%SCRIPT_PATH%" (
    echo [ERROR] random_rename.py was not found next to install.bat
    pause
    exit /b 1
)

echo Looking for Python...

for /f "delims=" %%i in ('where pythonw 2^>nul') do (
    if not defined PYW set "PYW=%%i"
)
if not defined PYW (
    for /f "delims=" %%i in ('where pyw 2^>nul') do (
        if not defined PYW set "PYW=%%i"
    )
)
if not defined PYW (
    for /f "delims=" %%i in ('where python 2^>nul') do (
        if not defined PYW set "PYW=%%i"
    )
)

if not defined PYW (
    echo [ERROR] Python was not found on this PC.
    echo Please install it from https://www.python.org/downloads/
    echo Make sure to check "Add python.exe to PATH" during setup, then run this file again.
    pause
    exit /b 1
)

echo Found Python: !PYW!

REM Save the resolved path so configure_settings.bat can reuse it
> "%SCRIPT_DIR%pythonw_path.txt" echo !PYW!

REM Find a console python.exe (not pythonw.exe) to run pip with, so any
REM install messages/errors are visible in this window. Falls back to
REM whatever we found above if a console python isn't available.
set "PYCONSOLE="
for /f "delims=" %%i in ('where python 2^>nul') do (
    if not defined PYCONSOLE set "PYCONSOLE=%%i"
)
if not defined PYCONSOLE set "PYCONSOLE=!PYW!"

echo.
echo Installing the "customtkinter" UI library (used by the settings window)...
"!PYCONSOLE!" -m pip install --user --upgrade customtkinter >nul 2>"%SCRIPT_DIR%pip_install_log.txt"
"!PYCONSOLE!" -c "import customtkinter" >nul 2>nul
if errorlevel 1 (
    echo [WARNING] Could not automatically install "customtkinter".
    echo The right-click "Random Rename" action will still work fine,
    echo but "configure_settings.bat" needs this package to open.
    echo.
    echo Try installing it manually with:
    echo   "!PYCONSOLE!" -m pip install customtkinter
    echo ^(see pip_install_log.txt in this folder for details^)
) else (
    del "%SCRIPT_DIR%pip_install_log.txt" >nul 2>nul
    echo Done.
)

echo.
echo Adding context menu entry...

REM ===== FILES ("*") =====
reg add "HKCU\Software\Classes\*\shell\RandomRenameQuick" /ve /d "Random Rename" /f >nul
reg add "HKCU\Software\Classes\*\shell\RandomRenameQuick" /v "Icon" /d "%ICON_QUICK%" /f >nul
reg add "HKCU\Software\Classes\*\shell\RandomRenameQuick\command" /ve /d "\"!PYW!\" \"%SCRIPT_PATH%\" --quick \"%%1\"" /f >nul

REM ===== FOLDERS =====
reg add "HKCU\Software\Classes\Directory\shell\RandomRenameQuick" /ve /d "Random Rename" /f >nul
reg add "HKCU\Software\Classes\Directory\shell\RandomRenameQuick" /v "Icon" /d "%ICON_QUICK%" /f >nul
reg add "HKCU\Software\Classes\Directory\shell\RandomRenameQuick\command" /ve /d "\"!PYW!\" \"%SCRIPT_PATH%\" --quick \"%%1\"" /f >nul

REM Remove the old "Customize..." entry from earlier versions, if present
reg delete "HKCU\Software\Classes\*\shell\RandomRenameCustom" /f >nul 2>nul
reg delete "HKCU\Software\Classes\Directory\shell\RandomRenameCustom" /f >nul 2>nul
reg delete "HKCU\Software\Classes\*\shell\RandomRename" /f >nul 2>nul
reg delete "HKCU\Software\Classes\Directory\shell\RandomRename" /f >nul 2>nul

echo.
echo ============================================
echo   INSTALLED SUCCESSFULLY!
echo   Right-click any file or folder -^> "Random Rename"
echo   renames it instantly, no window.
echo.
echo   To CHANGE how it renames things (character type,
echo   length, prefix, etc.), run "configure_settings.bat"
echo   to open the settings window.
echo ============================================
echo.
pause
