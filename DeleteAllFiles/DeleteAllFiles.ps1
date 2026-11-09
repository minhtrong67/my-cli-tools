param(
    [string]$Path = $env:DAF_PATH,
    [string]$Mode = $env:DAF_MODE
)
if ($Mode -ne 'Permanent') { $Mode = 'Recycle' }

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName Microsoft.VisualBasic
[System.Windows.Forms.Application]::EnableVisualStyles()

$title = 'Delete All Files'

function Show-Msg($text, $icon = 'Information', $buttons = 'OK', $default = 'Button1') {
    return [System.Windows.Forms.MessageBox]::Show($text, $title, $buttons, $icon, $default)
}

# ---- Config (config.json, optional) ----
$cfg = @{ confirm = $true; includeSubfolders = $false; showDoneMessage = $true }
$cfgFile = Join-Path $PSScriptRoot 'config.json'
if (Test-Path -LiteralPath $cfgFile) {
    try {
        $j = Get-Content -LiteralPath $cfgFile -Raw | ConvertFrom-Json
        foreach ($k in @($cfg.Keys)) { if ($null -ne $j.$k) { $cfg[$k] = $j.$k } }
    } catch { }
}

# ---- Resolve & protect ----
if ([string]::IsNullOrWhiteSpace($Path) -or -not [IO.Path]::IsPathRooted($Path)) {
    Show-Msg "No valid folder path received.`nReceived: '$Path'" 'Error' | Out-Null
    exit 1
}
try { $full = [IO.Path]::GetFullPath($Path).TrimEnd('\') } catch { Show-Msg "Invalid path:`n$Path" 'Error' | Out-Null; exit 1 }
if (-not (Test-Path -LiteralPath $full -PathType Container)) { Show-Msg "Folder not found:`n$full" 'Error' | Out-Null; exit 1 }

$root = [IO.Path]::GetPathRoot($full).TrimEnd('\')
$protected = @(
    $env:SystemRoot, $env:ProgramFiles, ${env:ProgramFiles(x86)}, $env:ProgramData, $env:USERPROFILE
) | Where-Object { $_ } | ForEach-Object { $_.TrimEnd('\') }

if ($full -eq $root -or ($protected -contains $full)) {
    Show-Msg "For safety, this folder is protected and cannot be cleared:`n$full" 'Stop' | Out-Null
    exit 1
}
foreach ($p in @($env:SystemRoot, $env:ProgramFiles, ${env:ProgramFiles(x86)})) {
    if ($p -and $full.StartsWith($p.TrimEnd('\') + '\', [StringComparison]::OrdinalIgnoreCase)) {
        Show-Msg "For safety, system folders are protected:`n$full" 'Stop' | Out-Null
        exit 1
    }
}

# ---- Collect files ----
$gci = @{ LiteralPath = $full; File = $true; Force = $true; ErrorAction = 'SilentlyContinue' }
if ($cfg.includeSubfolders) { $gci.Recurse = $true }
$files = @(Get-ChildItem @gci)

if ($files.Count -eq 0) {
    Show-Msg "No files to delete in:`n$full" | Out-Null
    exit 0
}

$sizeMB = [math]::Round((($files | Measure-Object Length -Sum).Sum) / 1MB, 2)
$scope  = if ($cfg.includeSubfolders) { ' (including subfolders)' } else { '' }

# ---- Confirm ----
if ($cfg.confirm) {
    if ($Mode -eq 'Permanent') {
        $msg = "PERMANENTLY delete $($files.Count) file(s) ($sizeMB MB)$scope in:`n$full`n`nThis CANNOT be undone and files will NOT go to the Recycle Bin."
        $r = Show-Msg $msg 'Warning' 'YesNo' 'Button2'
    } else {
        $msg = "Move $($files.Count) file(s) ($sizeMB MB)$scope to the Recycle Bin from:`n$full"
        $r = Show-Msg $msg 'Question' 'YesNo' 'Button1'
    }
    if ($r -ne 'Yes') { exit 0 }
}

# ---- Delete ----
$ok = 0; $fail = @()
foreach ($f in $files) {
    try {
        if ($Mode -eq 'Permanent') {
            Remove-Item -LiteralPath $f.FullName -Force -ErrorAction Stop
        } else {
            [Microsoft.VisualBasic.FileIO.FileSystem]::DeleteFile(
                $f.FullName,
                [Microsoft.VisualBasic.FileIO.UIOption]::OnlyErrorDialogs,
                [Microsoft.VisualBasic.FileIO.RecycleOption]::SendToRecycleBin,
                [Microsoft.VisualBasic.FileIO.UICancelOption]::ThrowException)
        }
        $ok++
    } catch {
        $fail += $f.Name
    }
}

if ($cfg.showDoneMessage -or $fail.Count -gt 0) {
    $verb = if ($Mode -eq 'Permanent') { 'deleted permanently' } else { 'moved to the Recycle Bin' }
    $text = "$ok file(s) $verb."
    if ($fail.Count -gt 0) {
        $list = ($fail | Select-Object -First 10) -join "`n"
        $text += "`n`n$($fail.Count) file(s) failed (in use or no permission):`n$list"
        if ($fail.Count -gt 10) { $text += "`n..." }
        Show-Msg $text 'Warning' | Out-Null
    } else {
        Show-Msg $text | Out-Null
    }
}
