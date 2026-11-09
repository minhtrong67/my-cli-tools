Option Explicit
Dim args, folder, mode, fso, dir, ps, cmd, sh
Set args = WScript.Arguments
If args.Count < 2 Then WScript.Quit 1
folder = args(0)
mode = args(1)
Set fso = CreateObject("Scripting.FileSystemObject")
dir = fso.GetParentFolderName(WScript.ScriptFullName)
ps = dir & "\DeleteAllFiles.ps1"
Set sh = CreateObject("WScript.Shell")
' Pass values via environment variables to avoid command-line quoting problems
With sh.Environment("Process")
    .Item("DAF_PATH") = folder
    .Item("DAF_MODE") = mode
End With
cmd = "powershell.exe -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File """ & ps & """"
sh.Run cmd, 0, False
