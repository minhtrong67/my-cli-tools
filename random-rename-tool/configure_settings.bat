@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_PATH=%SCRIPT_DIR%random_rename.py"
set "PYW="

if not exist "%SCRIPT_PATH%" (
    echo [ERROR] random_rename.py was not found next to configure_settings.bat
    pause
    exit /b 1
)

REM Reuse the Python path found by install.bat, if available
if exist "%SCRIPT_DIR%pythonw_path.txt" (
    set /p PYW=<"%SCRIPT_DIR%pythonw_path.txt"
)

if not defined PYW (
    for /f "delims=" %%i in ('where pythonw 2^>nul') do (
        if not defined PYW set "PYW=%%i"
    )
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
    echo Please run install.bat first, or install Python from
    echo https://www.python.org/downloads/ ^(check "Add python.exe to PATH"^).
    pause
    exit /b 1
)

start "" "!PYW!" "%SCRIPT_PATH%" --settings
