@echo off
title ASP.NET Core - Bin Folder Cleaner

REM === Goi PowerShell doc chinh file .bat nay va chay phan PS ben duoi ===
powershell -NoLogo -NoProfile -ExecutionPolicy Bypass -Command "$c = Get-Content -LiteralPath '%~f0' -Raw; $i = $c.LastIndexOf('#:PSBEGIN'); if ($i -lt 0) { Write-Host 'ERROR: marker not found' -ForegroundColor Red } else { Invoke-Expression $c.Substring($i) }"

echo.
echo ============================================
echo Nhan phim bat ky de thoat...
pause >nul
exit /b

#:PSBEGIN
$ErrorActionPreference = 'Continue'

# ============================================================
# FOLDER SE BI BO QUA (frontend / khong lien quan dotnet)
# ============================================================
$SkipFolders = @(
    'node_modules',
    '.git',
    '.vs',
    '.vscode',
    '.idea',
    'dist',
    'build',
    '.next',
    '.nuxt',
    '.angular',
    '.cache',
    'coverage',
    '.turbo',
    '.parcel-cache',
    'vendor',
    'packages'
)

function Write-Color {
    param([string]$Text, [string]$Color = 'White')
    Write-Host $Text -ForegroundColor $Color
}

function Format-Size {
    param([long]$Bytes)
    if ($Bytes -ge 1GB) { return ('{0:N2} GB' -f ($Bytes / 1GB)) }
    if ($Bytes -ge 1MB) { return ('{0:N2} MB' -f ($Bytes / 1MB)) }
    if ($Bytes -ge 1KB) { return ('{0:N2} KB' -f ($Bytes / 1KB)) }
    return "$Bytes B"
}

function Get-FolderSize {
    param([string]$Path)
    try {
        $sum = (Get-ChildItem -LiteralPath $Path -Recurse -Force -File -ErrorAction SilentlyContinue |
                Measure-Object -Property Length -Sum).Sum
        if ($null -eq $sum) { return [long]0 }
        return [long]$sum
    } catch { return [long]0 }
}

function Remove-BinFolder {
    param([string]$Path)
    try {
        Remove-Item -LiteralPath $Path -Recurse -Force -ErrorAction Stop
        return @{ Ok = $true; Error = $null }
    } catch {
        $null = cmd /c "rmdir /s /q `"$Path`"" 2>&1
        if (-not (Test-Path -LiteralPath $Path)) {
            return @{ Ok = $true; Error = $null }
        }
        return @{ Ok = $false; Error = $_.Exception.Message }
    }
}

function Show-Items {
    param($List)
    Write-Host ''
    Write-Color ('{0,-5} {1,-12} {2}' -f '#', 'Size', 'Path') 'Cyan'
    Write-Color ('-' * 110) 'DarkGray'
    foreach ($it in $List) {
        Write-Host ('{0,-5} {1,-12} {2}' -f $it.Index, $it.SizeStr, $it.Path)
    }
    Write-Color ('-' * 110) 'DarkGray'
    $total = 0L
    foreach ($it in $List) { $total += [long]$it.Size }
    Write-Color ("Tong: $($List.Count) folder - " + (Format-Size $total)) 'Yellow'
}

# ============================================================
# QUET DE QUY, TU BO QUA CAC FOLDER TRONG $SkipFolders
# ============================================================
function Find-BinFolders {
    param([string]$RootPath)

    $result = New-Object System.Collections.ArrayList
    $queue  = New-Object System.Collections.Generic.Queue[string]
    $queue.Enqueue($RootPath)

    while ($queue.Count -gt 0) {
        $current = $queue.Dequeue()

        $subDirs = @()
        try {
            $subDirs = Get-ChildItem -LiteralPath $current -Force -Directory -ErrorAction SilentlyContinue
        } catch { continue }

        foreach ($d in $subDirs) {
            $name = $d.Name.ToLowerInvariant()

            if ($SkipFolders -contains $name) { continue }

            if ($name -eq 'bin') {
                [void]$result.Add($d.FullName)
                continue
            }

            $queue.Enqueue($d.FullName)
        }
    }

    return ,$result.ToArray()
}

try {
    Clear-Host
    Write-Color '======================================================' 'Cyan'
    Write-Color '        ASP.NET CORE - BIN FOLDER CLEANER            ' 'Cyan'
    Write-Color '        Quet & xoa folder bin trong source           ' 'Cyan'
    Write-Color '        (Bo qua node_modules, dist, build, .git...)  ' 'Cyan'
    Write-Color '======================================================' 'Cyan'

    $Root = Read-Host "`nNhap duong dan can quet (Enter de dung D:\)"
    if ([string]::IsNullOrWhiteSpace($Root)) { $Root = 'D:\' }

    if (-not (Test-Path -LiteralPath $Root)) {
        Write-Color "LOI: Duong dan khong ton tai: $Root" 'Red'
        return
    }
    $Root = (Resolve-Path -LiteralPath $Root).Path

    Write-Color "`n[1/2] Dang quet '$Root' ..." 'Yellow'
    Write-Color ("      Bo qua: " + ($SkipFolders -join ', ')) 'DarkGray'

    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    $topBins = Find-BinFolders -RootPath $Root
    $sw.Stop()

    $binCount = 0
    if ($null -ne $topBins) { $binCount = @($topBins).Count }

    Write-Color ("      Hoan tat trong {0:N2}s" -f $sw.Elapsed.TotalSeconds) 'Green'
    Write-Color ("      Tim thay {0} folder 'bin'" -f $binCount) 'Green'

    if ($binCount -eq 0) {
        Write-Color "`nKhong co folder 'bin' nao de xoa." 'Yellow'
        return
    }

    Write-Color "`n[2/2] Dang tinh dung luong..." 'Yellow'

    # Dung array thuong thay vi List<object> de tranh loi binding
    $items = @()
    $idx = 0
    foreach ($p in @($topBins)) {
        $idx++
        $sz = Get-FolderSize $p
        $items += [pscustomobject]@{
            Index   = $idx
            Path    = $p
            Size    = $sz
            SizeStr = Format-Size $sz
        }
    }

    while ($true) {
        Show-Items $items
        Write-Host ''
        Write-Color 'Lenh:' 'Cyan'
        Write-Host '  d <so>     Xoa theo so  (vd: d 1   hoac   d 1,3,5)'
        Write-Host '  all        Xoa toan bo danh sach'
        Write-Host '  q          Thoat'

        $cmd = Read-Host 'Nhap lenh'
        if ([string]::IsNullOrWhiteSpace($cmd)) { continue }
        $cmd = $cmd.Trim()

        if ($cmd -ieq 'q' -or $cmd -ieq 'quit' -or $cmd -ieq 'exit') {
            Write-Color 'Thoat.' 'Cyan'
            break
        }

        $targets = @()

        if ($cmd -ieq 'all') {
            $targets = @($items)
        }
        elseif ($cmd -match '^(?i)d\s+(.+)$') {
            $spec = $Matches[1].Trim()
            $nums = @($spec -split '[,\s]+' | Where-Object { $_ -match '^\d+$' } | ForEach-Object { [int]$_ })
            foreach ($n in $nums) {
                $hit = @($items | Where-Object { [int]$_.Index -eq $n })
                if ($hit.Count -gt 0) {
                    $targets += $hit
                } else {
                    Write-Color "  ! Khong co muc so $n" 'Red'
                }
            }
        }
        else {
            Write-Color '  ! Lenh khong hop le.' 'Red'
            continue
        }

        if (@($targets).Count -eq 0) { continue }

        Write-Host ''
        Write-Color "Se xoa $(@($targets).Count) folder:" 'Yellow'
        foreach ($t in $targets) { Write-Host "   - $($t.Path)" }
        $confirm = Read-Host 'Xac nhan? (y/N)'
        if ($confirm -notmatch '^(?i)y') { Write-Color '  Da huy.' 'DarkYellow'; continue }

        $deleted = 0
        $failed  = 0
        $deletedPaths = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::OrdinalIgnoreCase)

        foreach ($t in $targets) {
            Write-Host -NoNewline "  Xoa: $($t.Path) ... "
            $r = Remove-BinFolder -Path $t.Path
            if ($r.Ok) {
                Write-Color 'OK' 'Green'
                $deleted++
                [void]$deletedPaths.Add($t.Path)
            } else {
                Write-Color "THAT BAI - $($r.Error)" 'Red'
                $failed++
            }
        }

        # Rebuild danh sach: loai bo path da xoa + danh so lai tu 1
        $newItems = @()
        $i = 0
        foreach ($it in $items) {
            if ($deletedPaths.Contains($it.Path)) { continue }
            $i++
            $newItems += [pscustomobject]@{
                Index   = $i
                Path    = $it.Path
                Size    = $it.Size
                SizeStr = $it.SizeStr
            }
        }
        $items = $newItems

        Write-Host ''
        Write-Color "Ket qua: xoa thanh cong $deleted, that bai $failed." 'Yellow'

        if (@($items).Count -eq 0) {
            Write-Color 'Da xoa het folder bin.' 'Green'
            break
        }
    }
}
catch {
    Write-Host ''
    Write-Color "LOI KHONG MONG MUON: $($_.Exception.Message)" 'Red'
    Write-Color $_.ScriptStackTrace 'DarkGray'
}