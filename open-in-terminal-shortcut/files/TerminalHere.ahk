#Requires AutoHotkey v2.0
#SingleInstance Force
Persistent
SetWorkingDir A_ScriptDir

cfg := A_ScriptDir "\config.ini"
curHK := ""
hk := IniRead(cfg, "main", "hotkey", "CTRL+ALT+T")
Register(hk)

A_TrayMenu.Delete()
A_TrayMenu.Add("Đổi phím tắt…", ChangeHK)
A_TrayMenu.Add("Thoát", (*) => ExitApp())
A_TrayMenu.Default := "Đổi phím tắt…"

ToAhk(s) {
    out := "", key := ""
    for part in StrSplit(StrUpper(s), "+") {
        part := Trim(part)
        if (part = "CTRL" || part = "CONTROL")
            out .= "^"
        else if (part = "ALT")
            out .= "!"
        else if (part = "SHIFT")
            out .= "+"
        else if (part = "WIN")
            out .= "#"
        else
            key := part
    }
    return out . key
}

Register(s) {
    global curHK
    try {
        if (curHK != "")
            Hotkey(curHK, "Off")
    }
    try {
        curHK := ToAhk(s)
        Hotkey(curHK, OpenHere, "On")
        A_IconTip := "Terminal Here: " s
    } catch {
        curHK := ""
        MsgBox("Phím tắt không hợp lệ: " s, "Terminal Here")
        return false
    }
    return true
}

ChangeHK(*) {
    global cfg
    r := InputBox("Nhập phím tắt, ví dụ CTRL+ALT+T, CTRL+SHIFT+T, WIN+T, ALT+F1", "Terminal Here", "w380 h130", IniRead(cfg, "main", "hotkey", "CTRL+ALT+T"))
    if (r.Result = "OK" && Trim(r.Value) != "" && Register(Trim(r.Value)))
        IniWrite(Trim(r.Value), cfg, "main", "hotkey")
}

PathOf(w) {
    sel := w.Document.SelectedItems()
    if (sel.Count >= 1) {
        p := sel.Item(0).Path
        if DirExist(p)
            return p
        SplitPath(p, , &dir)
        return dir
    }
    return w.Document.Folder.Self.Path
}

GetFolder() {
    hwnd := WinExist("A")
    cls := WinGetClass("A")
    if (cls = "CabinetWClass") {
        tab := 0
        try tab := ControlGetHwnd("ShellTabWindowClass1", hwnd)
        first := ""
        for w in ComObject("Shell.Application").Windows {
            try {
                if (w.HWND != hwnd)
                    continue
                if (first = "")
                    first := PathOf(w)
                if (tab) {
                    sb := ComObjQuery(w, "{4C96BE40-915C-11CF-99D3-00AA004AE837}", "{000214E2-0000-0000-C000-000000000046}")
                    t := 0
                    DllCall(NumGet(NumGet(sb.ptr, 0, "ptr") + 3 * A_PtrSize, "ptr"), "ptr", sb, "ptr*", &t)
                    if (t = tab)
                        return PathOf(w)
                }
            }
        }
        if (first != "")
            return first
    } else if (cls = "Progman" || cls = "WorkerW") {
        return A_Desktop
    }
    return EnvGet("USERPROFILE")
}

OpenHere(*) {
    path := GetFolder()
    if !DirExist(path)
        path := EnvGet("USERPROFILE")
    if (SubStr(path, -1) = "\")
        path .= "\"
    if FileExist(EnvGet("LOCALAPPDATA") "\Microsoft\WindowsApps\wt.exe")
        Run('wt.exe -d "' path '"')
    else
        Run("powershell.exe -NoExit", path)
}
