@echo off
chcp 65001 >nul
title Tai MP3 tu YouTube
cd /d "%~dp0"

REM ---- Kiem tra Python ----
where python >nul 2>nul
if errorlevel 1 (
    echo [LOI] Khong tim thay Python trong PATH.
    echo Hay cai Python tai https://www.python.org/downloads/
    echo Nho tick "Add Python to PATH" khi cai dat.
    pause
    exit /b 1
)

REM ---- Kiem tra yt-dlp: chi cai/cap nhat neu chua co, khong lam phien neu da san sang ----
python -c "import yt_dlp" >nul 2>nul
if errorlevel 1 (
    echo Dang cai dat yt-dlp lan dau, vui long doi...
    pip install -U yt-dlp >nul 2>nul
)

REM ---- Kiem tra ffmpeg (chi bao khi thieu) ----
where ffmpeg >nul 2>nul
if errorlevel 1 (
    echo.
    echo [CANH BAO] Chua tim thay ffmpeg - can de chuyen sang MP3.
    echo Cai bang: winget install ffmpeg
    echo Hoac tai tai: https://www.gyan.dev/ffmpeg/builds/
    echo.
    pause
    exit /b 1
)

python download_mp3.py
