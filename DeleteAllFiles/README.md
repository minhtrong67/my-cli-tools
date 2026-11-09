# Delete All Files (Windows context menu)

Right-click a folder (or empty space inside an open folder) > **Delete all files**:
- **Move to Recycle Bin (default)**: recoverable
- **Delete permanently**: skips the Recycle Bin (confirmation defaults to "No")

## Install / Uninstall
Double-click `Install.bat` / `Uninstall.bat`. No admin rights needed (HKCU).
Windows 11: the entry is under "Show more options".

## config.json (in %LOCALAPPDATA%\DeleteAllFiles)
- `confirm`: show a confirmation dialog (default true)
- `includeSubfolders`: also delete files inside subfolders (default false; subfolders themselves are never removed)
- `showDoneMessage`: show a result dialog (default true)

## Safety
Drive roots, Windows, Program Files, ProgramData and the user profile root are blocked.
