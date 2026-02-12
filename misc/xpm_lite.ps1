#! /usr/bin/env pwsh
<#
.SYNOPSIS
    A simple script to install xPack packages from GitHub releases.
.PARAMETER Xpacks
    A hashtable or JSON string where keys are the names of xPack packages (e.g. @xpack-dev-tools/riscv-none-elf-gcc) and values are the versions to install (e.g. 14.2.0-3).
.PARAMETER Path
    The path to install the packages to. Defaults to ApplicationData for normal users, or CommonApplicationData for admins.
.PARAMETER SkipPrompts
    If enabled, will answer yes to all prompts.
.PARAMETER Platform
    The platform identifier to use, overriding auto-detection. Examples: win32-x64, darwin-x64, linux-x64, etc.
.EXAMPLE
    .\install_xpack.ps1 @{ '@xpack-dev-tools/riscv-none-elf-gcc' = '14.2.0-3'; '@xpack-dev-tools/openocd' = '0.12.0-1' }
    This will install the specified versions of the RISC-V GCC toolchain and OpenOCD to the default location.
#>
param (
    [Parameter(Position = 0, Mandatory = $true, ValueFromPipeline = $true)]
    $Xpacks,
    [string]$Path,
    [switch]$SkipPrompts,
    [string]$Platform
);

$ErrorActionPreference = 'Stop';

function ObjectToHashtable($obj) {
    $hash = @{}
    $obj.psobject.properties | ForEach-Object { $Hash."$($_.Name)" = $_.Value }
    return $hash
}

function Get-Platform {
    if ($Platform) { return $Platform }
    $rid = [System.Runtime.InteropServices.RuntimeInformation]::RuntimeIdentifier;
    switch -Wildcard ($rid) {
        'win*' { return $rid -replace '^win', 'win32' }
        'osx*' { return $rid -replace '^osx', 'darwin' }
        $null { return 'win32-x64' }
        '*' { return $rid }
    }
}

function Get-IsAdmin {
    switch ($PSVersionTable.Platform) {
        'Unix' {
            return ((id -u) -eq 0)
        }
        Default {
            $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
            $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
            return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
        }
    }
}

function Get-InstallPath {
    if ($Path) { return $Path }
    $specialFolder = if (Get-IsAdmin) { 'CommonApplicationData' } else { 'LocalApplicationData' }
    return Join-Path $([Environment]::GetFolderPath($specialFolder)) 'xpacks'
}

function Prompt-YesNo($message) {
    if ($SkipPrompts) { return $true }
    while ($true) {
        $response = Read-Host "$message (y/n)"
        switch ($response.ToLower()) {
            'y' { return $true }
            'n' { return $false }
            Default { Write-Host "Please enter 'y' or 'n'." }
        }
    }
}

function MkDir($path) {
    New-Item -ItemType Directory -Path $path -ErrorAction SilentlyContinue | Out-Null
}

if ($Xpacks -is [string]) {
    $Xpacks = ObjectToHashtable (ConvertFrom-Json $Xpacks)
} elseif (-not ($Xpacks -is [hashtable])) {
    throw 'Xpacks parameter must be a hashtable or JSON string'
}

$cyan = "$([char]0x1b)[36m"
$reset = "$([char]0x1b)[0m"

$platform = Get-Platform
Write-Host "➤ Using platform $cyan$platform$reset"

$archive = "tar.gz"
if ($platform -like 'win*') {
    $archive = "zip"
}

$packageCount = $Xpacks.Count
$installPath = Get-InstallPath
Write-Host "➤ Installing $cyan$packageCount$reset packages to $cyan$installPath$reset"
if (-not (Prompt-YesNo "Is this correct?")) {
    Write-Host "Installation cancelled."
    exit
}

$tempFolder = [System.IO.Path]::GetTempPath();

$ProgressPreference = 'SilentlyContinue'
foreach ($xpack in $Xpacks.GetEnumerator()) {
    $fullName = $xpack.Key
    $version = $xpack.Value
    $name = $fullName.Split('/')[-1]
    Write-Host "➤ Installing $cyan$name$reset version $cyan$version$reset"
    $filename = "$name-$version-$platform.$archive"
    $downloadUrl = "https://github.com/$($fullName -replace '^@', '')-xpack/releases/download/v$version/xpack-$filename"
    $downloadPath = Join-Path $tempFolder $filename
    Write-Host "➤ Downloading from $cyan$downloadUrl$reset..."
    try {
        Invoke-WebRequest -Uri $downloadUrl -OutFile $downloadPath
    } catch {
        Write-Host "✘ Failed to download: $_"
        continue
    }
    Write-Host "➤ Extracting archive..."
    if ($archive -eq 'zip') {
        Expand-Archive -Path $downloadPath -DestinationPath $installPath -Force
    } else {
        MkDir $installPath
        tar -xzf $downloadPath -C $installPath
        if ($LASTEXITCODE -ne 0) {
            Write-Host "✘ Failed to extract archive"
            continue
        }
    }
    # Remove the top-level folder created by the archive, if it exists
    Write-Host "➤ Cleaning up extracted files..."
    $extractedFolder = Join-Path $installPath "xpack-$name-$version"
    if (Test-Path $extractedFolder) {
        Get-ChildItem -Path $extractedFolder -Recurse -File | Move-Item -Destination {
            $relativePath = $_.FullName.Substring($extractedFolder.Length).TrimStart('\', '/')
            MkDir (Join-Path $installPath (Split-Path $relativePath))
            Join-Path $installPath $relativePath
        } -Force
        Remove-Item -Path $extractedFolder -Recurse -Force
    }
    Write-Host "✔ Install completed"
}
$ProgressPreference = 'Continue'

$binPath = Join-Path $installPath 'bin'
$target = if (Get-IsAdmin) { [EnvironmentVariableTarget]::Machine } else { [EnvironmentVariableTarget]::User }
$systemPath = [Environment]::GetEnvironmentVariable('PATH', $target)
if (-not ($systemPath -split ';' | Where-Object { $_ -eq $binPath })) {
    if (Prompt-YesNo "Do you want to add $binPath to your PATH environment variable? This will allow you to run the installed tools from any terminal.") {
        Write-Host "➤ Adding $cyan$binPath$reset to PATH"
        [Environment]::SetEnvironmentVariable('PATH', "$systemPath;$binPath", $target)
        $env:PATH = "$env:PATH;$binPath"
        Write-Host "✔ PATH updated!"
    }
} else {
    Write-Host "➤ $cyan$binPath$reset is already in PATH, skipping."
}
Write-Host "✔ All done!"
