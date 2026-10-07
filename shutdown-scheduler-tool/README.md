# Hen Gio Tat May (Shutdown Scheduler)

A lightweight, native Windows desktop application that automatically shuts down your computer at a specific time or after a countdown, with daily repeat support and pre-shutdown notifications. Built directly on the Win32 API in C, with no .NET, Electron, or other runtime dependencies required.

Bilingual interface: Vietnamese and English, switchable at any time from within the app.

## Features

- **Two scheduling modes**: shut down at a specific time of day (HH:MM), or after a countdown (hours and minutes).
- **Daily repeat**: automatically re-arms the schedule every day. Works across reboots by launching with Windows and re-arming itself on startup.
- **Pre-shutdown notification**: a system tray balloon warns you a configurable number of minutes before shutdown, so you can cancel in time.
- **System tray integration**: closing the window keeps the app running quietly in the tray while a schedule is active; double-click the tray icon to reopen it.
- **Bilingual UI**: toggle between Vietnamese and English with one click. The choice is remembered between sessions.
- **Adaptive interface**: fonts and layout are derived from the operating system's own UI font and metrics, so spacing and text rendering (including Vietnamese diacritics) stay correct across different Windows versions, DPI settings, and font sizes.
- **No installation required**: a single, self-contained `.exe` file with no external dependencies.

## Requirements

- Windows 10 or Windows 11 (x64).
- No .NET Runtime, Java, or Node.js needed to run the application.

## Installation

1. Download `hen-gio-tat-may.exe`.
2. Run it directly. No installer or admin rights are required.
3. Windows SmartScreen may show a warning because the executable is unsigned. Choose "More info" then "Run anyway" to proceed.

## Usage

1. Choose a scheduling mode: **At a specific time** or **Countdown**.
2. Enter the target time (or the number of hours/minutes for the countdown).
3. Optionally enable:
   - **Repeat daily**, to have the app re-arm the schedule automatically every day.
   - **Launch with Windows**, required for daily repeat to survive a reboot.
   - **Force close open applications**, to force shutdown even if programs have unsaved changes.
4. Set how many minutes in advance you want to be notified before shutdown.
5. Click **Start**. A countdown will appear, and a tray balloon will notify you before the machine shuts down.
6. Click **Cancel** at any time to stop the scheduled shutdown.

Settings are stored per user in `%APPDATA%\HenGioTatMay.ini`.

## Building from source

The application is written in portable Win32 C and is cross-compiled from Linux using MinGW-w64. No Windows machine is required to build it.

### Prerequisites

```bash
sudo apt-get update
sudo apt-get install -y gcc-mingw-w64-x86-64 binutils-mingw-w64-x86-64
```

### Build

```bash
x86_64-w64-mingw32-windres app.rc -O coff -o app_res.o
x86_64-w64-mingw32-gcc -O2 -mwindows -municode -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -o hen-gio-tat-may.exe shutdown_scheduler.c app_res.o -lcomctl32 -lshell32 -ladvapi32
```

The `app.rc` / `app.manifest` pair embeds a manifest that enables Common Controls v6 (modern themed controls) and per-monitor DPI awareness.

## Project structure

```
shutdown_scheduler.c   Application source (Win32 API, single file)
app.manifest            Application manifest (Common Controls v6, DPI awareness)
app.rc                  Resource script embedding the manifest into the executable
hen-gio-tat-may.exe      Compiled executable
```

## Technical notes

- UI text is stored in parallel Vietnamese/English string tables and rendered as UTF-16, ensuring correct display of Vietnamese diacritics.
- Fonts are obtained from `SystemParametersInfo(SPI_GETNONCLIENTMETRICS)` rather than hard-coded, so the app always matches the OS's actual UI font (for example, Segoe UI Variable on Windows 11).
- Control heights and spacing are computed at runtime from measured font metrics, avoiding text clipping regardless of system font, DPI, or language.
- Shutdown scheduling is delegated to the native Windows `shutdown` command; the application does not require elevated privileges beyond what `shutdown` itself needs.

## Known limitations

- Daily repeat relies on the application auto-launching at Windows startup to re-arm the next occurrence; if the computer is not restarted or the app is not allowed to launch at startup, the daily cycle will pause until the next launch.
- The executable is unsigned, so Windows SmartScreen may flag it on first run.

## License

This project is provided as-is for personal use. No warranty is expressed or implied.

## Author

**minhtrong67**

## Acknowledgments

Developed with the support of Claude (Anthropic).
