param(
    [Parameter(Mandatory = $true)][string]$Action,
    [Parameter(Mandatory = $true)][string]$Path
)

# Converter Tool - chay boi Windows context menu
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

$ErrorActionPreference = 'Stop'
$AppName = 'Converter Tool'

function Show-Error([string]$msg) {
    [System.Windows.Forms.MessageBox]::Show($msg, $AppName, 'OK', 'Error') | Out-Null
}

function Show-Done([string]$msg) {
    $n = New-Object System.Windows.Forms.NotifyIcon
    $n.Icon = [System.Drawing.SystemIcons]::Information
    $n.BalloonTipTitle = $AppName
    $n.BalloonTipText = $msg
    $n.Visible = $true
    $n.ShowBalloonTip(3000)
    Start-Sleep -Milliseconds 3200
    $n.Dispose()
}

function Find-Ffmpeg {
    $local = Join-Path $PSScriptRoot 'ffmpeg.exe'
    if (Test-Path $local) { return $local }
    $cmd = Get-Command ffmpeg.exe -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    # winget thuong dat o day nhung PATH chua cap nhat trong session cu
    $wg = Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages" -Recurse -Filter ffmpeg.exe -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($wg) { return $wg.FullName }
    return $null
}

function Get-OutPath([string]$src, [string]$ext) {
    $dir = Split-Path $src -Parent
    $base = [IO.Path]::GetFileNameWithoutExtension($src)
    $out = Join-Path $dir "$base.$ext"
    $i = 1
    while (Test-Path -LiteralPath $out) {
        $out = Join-Path $dir "$base ($i).$ext"
        $i++
    }
    return $out
}

function Invoke-Ffmpeg([string]$ff, [string[]]$ffArgs) {
    $log = [IO.Path]::GetTempFileName()
    try {
        $ErrorActionPreference = 'Continue'
        & $ff @ffArgs *> $log
        $code = $LASTEXITCODE
        $ErrorActionPreference = 'Stop'
        if ($code -ne 0) {
            $tail = (Get-Content $log -Tail 6) -join "`n"
            throw "ffmpeg loi (ma $code):`n$tail"
        }
    }
    finally { Remove-Item $log -ErrorAction SilentlyContinue }
}

function Convert-WithDotNet([string]$src, [string]$dst, [string]$fmt) {
    $img = [System.Drawing.Image]::FromFile($src)
    try {
        $formats = @{
            png  = [System.Drawing.Imaging.ImageFormat]::Png
            jpg  = [System.Drawing.Imaging.ImageFormat]::Jpeg
            bmp  = [System.Drawing.Imaging.ImageFormat]::Bmp
            gif  = [System.Drawing.Imaging.ImageFormat]::Gif
            tiff = [System.Drawing.Imaging.ImageFormat]::Tiff
        }
        if ($fmt -eq 'jpg' -or $fmt -eq 'bmp') {
            # JPG/BMP khong co alpha: do nen trang
            $bmp = New-Object System.Drawing.Bitmap $img.Width, $img.Height
            $g = [System.Drawing.Graphics]::FromImage($bmp)
            $g.Clear([System.Drawing.Color]::White)
            $g.DrawImage($img, 0, 0, $img.Width, $img.Height)
            $g.Dispose()
            $bmp.Save($dst, $formats[$fmt])
            $bmp.Dispose()
        }
        else {
            $img.Save($dst, $formats[$fmt])
        }
    }
    finally { $img.Dispose() }
}

try {
    if (-not (Test-Path -LiteralPath $Path)) { throw "Khong tim thay file: $Path" }
    $Path = (Resolve-Path -LiteralPath $Path).ProviderPath
    $srcExt = [IO.Path]::GetExtension($Path).TrimStart('.').ToLower()
    $ff = Find-Ffmpeg

    $imageTargets = @('png', 'jpg', 'webp', 'bmp', 'gif', 'tiff', 'ico')

    if ($Action -in @('gif-small', 'gif-hq')) {
        if (-not $ff) { throw "Can ffmpeg de chuyen video sang GIF.`nChay lai install.bat hoac cai bang: winget install Gyan.FFmpeg" }
        if ($Action -eq 'gif-small') { $fps = 12; $w = 480 } else { $fps = 15; $w = 720 }
        $out = Get-OutPath $Path 'gif'
        $vf = "fps=$fps,scale=${w}:-1:flags=lanczos,split[s0][s1];[s0]palettegen=stats_mode=diff[p];[s1][p]paletteuse=dither=bayer:bayer_scale=5"
        Invoke-Ffmpeg $ff @('-y', '-i', $Path, '-vf', $vf, '-loop', '0', $out)
    }
    elseif ($Action -in $imageTargets) {
        $target = $Action
        if ($srcExt -eq $target -or ($srcExt -eq 'jpeg' -and $target -eq 'jpg') -or ($srcExt -eq 'tif' -and $target -eq 'tiff')) {
            throw "File da la dinh dang $($target.ToUpper())."
        }
        $out = Get-OutPath $Path $target
        $dotnetOk = @('png', 'jpg', 'bmp', 'gif', 'tiff') -contains $target
        $srcOk = @('png', 'jpg', 'jpeg', 'bmp', 'gif', 'tif', 'tiff', 'ico') -contains $srcExt

        if ($ff) {
            $a = @('-y', '-i', $Path)
            switch ($target) {
                'jpg'  { $a += @('-frames:v', '1', '-q:v', '2') }
                'webp' { $a += @('-frames:v', '1', '-quality', '90') }
                'ico'  { $a += @('-frames:v', '1', '-vf', 'scale=256:256:force_original_aspect_ratio=decrease,pad=256:256:(ow-iw)/2:(oh-ih)/2:color=0x00000000,format=bgra') }
                'gif'  { }
                default { $a += @('-frames:v', '1') }
            }
            $a += $out
            Invoke-Ffmpeg $ff $a
        }
        elseif ($dotnetOk -and $srcOk) {
            Convert-WithDotNet $Path $out $target
        }
        else {
            throw "Dinh dang nay can ffmpeg.`nChay lai install.bat de cai dat ffmpeg."
        }
    }
    else {
        throw "Action khong hop le: $Action"
    }

    Show-Done ("Da tao: " + (Split-Path $out -Leaf))
}
catch {
    Show-Error $_.Exception.Message
    exit 1
}
