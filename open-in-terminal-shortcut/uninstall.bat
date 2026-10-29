@echo off
taskkill /f /im TerminalHere.exe >nul 2>&1
del "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup\Terminal Here.lnk" >nul 2>&1
del "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Terminal Here.lnk" >nul 2>&1
rmdir /s /q "%LOCALAPPDATA%\TerminalHere" >nul 2>&1
echo Da go cai dat Terminal Here.
pause
