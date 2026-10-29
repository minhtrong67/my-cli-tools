RANDOM RENAME TOOL
===================

Adds ONE entry to the Windows right-click context menu:

  - "Random Rename"  -> renames the selected file/folder instantly, NO
                         window opens. Out of the box this uses a random
                         mix of letters + numbers (8 characters), keeping
                         the original file extension - no setup required.

Want it to rename things differently (only numbers, only letters,
a custom character set, a prefix, a fixed length, ...)? Run
"configure_settings.bat" any time to open a settings window, adjust
things, and click "Save". From then on, "Random Rename" in the
right-click menu will follow your saved configuration.

REQUIREMENTS
------------
- Windows 10/11
- Python installed (get it from https://www.python.org/downloads/)
  During setup, make sure to CHECK "Add python.exe to PATH" - this matters.
- The "customtkinter" library, used for the modern settings window.
  install.bat installs this automatically (needs internet the first time).
  If it can't, you can install it yourself with:
    python -m pip install customtkinter
  (Quick Rename from the right-click menu does NOT need this - it only
  uses Tkinter, which is already included with Python.)

INSTALL
-------
1. Extract the whole RandomRenameTool folder somewhere permanent
   (e.g. C:\Tools\RandomRenameTool). Don't use a temp folder - if you
   delete it later, the context menu entry will break.
2. Double-click "install.bat".
   (No administrator rights needed.)
3. Wait for the "INSTALLED SUCCESSFULLY!" message.

HOW TO USE
----------
Rename something (default settings, or whatever you last saved):
  1. Right-click a file or folder in File Explorer.
  2. Click "Random Rename".
  3. Done - it's renamed instantly.

Change how it renames things:
  1. Double-click "configure_settings.bat" (no need to select a file first -
     this just opens the settings window on its own).
  2. Adjust:
     - Character type:
         Numbers            -> e.g. 483920
         Letters             -> e.g. kxqpz
         Letters + Numbers   -> e.g. a8f3k2z9  (default)
         Custom              -> pick your own allowed characters,
                                 e.g. "ABCXYZ019"
     - Letter case (Letters / Letters+Numbers modes): lowercase,
       UPPERCASE, or MiXeD
     - Allow leading zero (Numbers mode only): on/off
     - Length: drag the slider, the example updates live
     - Prefix / Suffix: text added before/after the random value
       (e.g. prefix "IMG_" -> IMG_a8f3k2z9)
     - Keep file extension: keeps .jpg, .docx, etc. unchanged
     - Use one shared result for all: when you select multiple items at
       once, gives them all the same value instead of a separate one each
  3. Click "Shuffle again" to preview a different example, or
     "Reset to defaults" to go back to the original settings.
  4. Click "Save" to apply. A small "✓ Settings saved" confirmation appears
     at the bottom - the window stays open so you can keep experimenting.
     Click "Cancel" to close the window without saving further changes.
  5. From now on, right-click -> "Random Rename" uses these settings.

NOTE ABOUT SELECTING MULTIPLE ITEMS
------------------------------------
Depending on the Windows version, right-clicking multiple selected items
may run the rename once per item behind the scenes instead of a single
combined action - each item still gets renamed correctly, just processed
one after another.

UNINSTALL
---------
Double-click "uninstall.bat" to remove the "Random Rename" entry from
the right-click menu. This does not delete the tool's files or your
saved settings - you can delete the folder manually afterwards if you
no longer need it.

TROUBLESHOOTING
----------------
- "Python was not found": install Python from python.org, check
  "Add python.exe to PATH" during setup, then run install.bat again.
- The entry doesn't show up in the right-click menu: restart File
  Explorer (Task Manager -> find "Windows Explorer" -> Restart), or
  restart your PC.
- "already exists" warning after renaming: this only happens on a rare
  collision - just try again, or increase the length in the settings
  window for a lower chance of repeats.
