@echo off
setlocal
set "DIR=%LOCALAPPDATA%\TerminalHere"
set "HK=CTRL+ALT+T"
echo ==== Cai dat Terminal Here ====
echo Vi du: CTRL+ALT+T, CTRL+SHIFT+T, WIN+T, ALT+F1
set /p HK=Nhap phim tat (Enter = CTRL+ALT+T): 
taskkill /f /im TerminalHere.exe >nul 2>&1
del "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Terminal Here.lnk" >nul 2>&1
rmdir /s /q "%DIR%" >nul 2>&1
mkdir "%DIR%"
copy /y "%~dp0files\*" "%DIR%\" >nul
> "%DIR%\config.ini" echo [main]
>> "%DIR%\config.ini" echo hotkey=%HK%
powershell -NoProfile -Command "$s=(New-Object -ComObject WScript.Shell).CreateShortcut([Environment]::GetFolderPath('Startup')+'\Terminal Here.lnk'); $s.TargetPath='%DIR%\TerminalHere.exe'; $s.WorkingDirectory='%DIR%'; $s.Save()"
start "" "%DIR%\TerminalHere.exe"
echo.
echo Xong! Phim tat: %HK%
echo App chay nen o khay he thong. Chuot phai bieu tuong de doi phim tat.
pause
