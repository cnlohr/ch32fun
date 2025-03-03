# Installer script based on TinyCC installer script by Macyler (CaiB) from https://raw.githubusercontent.com/cntools/Install-TCC
param
(
    # The folder where GCC will be installed to
    [Parameter(Position = 0, Mandatory = $false)]
    [string]$Destination,

    # If enabled, will prevent addition of GCC to your PATH environment variable
    [switch]$NoPath,

    # Skips all prompts and just installs, mainly intended for use if we need to re-run as admin.
    [switch]$SkipPrompts
);

$ErrorActionPreference = 'Stop';
$XpackVersion = 'xpack-riscv-none-elf-gcc-14.2.0-3';
$MainDownloadZipName = "$XpackVersion-win32-x64.zip";
$MAIN_DOWNLOAD = "https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v14.2.0-3/$MainDownloadZipName";
$Make_Download = "https://cosmo.zip/pub/cosmos/bin/make"

Add-Type -Assembly 'System.IO.Compression.FileSystem';

if ([string]::IsNullOrEmpty($Destination)) { $Destination = (Join-Path $ENV:USERPROFILE (Join-Path 'AppData/Local' $XpackVersion ) ); }

if (-NOT $SkipPrompts)
{
    Write-Host "$XpackVersion will be installed to $Destination";
    if ($NoPath) { Write-Host 'TCC will NOT be added to your path environment variable.'; }
    else { Write-Host "$XpackVersion will be added to your user path environment variable."; }
    [string]$Answer = Read-Host -Prompt "Is this correct? (y/n)";
    if ($Answer -NE 'y') { Exit; }
}

Write-Host "Downloading $MainDownloadZipName";
Write-Host "(Showing progress here makes it download very slowly for some reason)";

$ProgressPreference = 'SilentlyContinue'
Invoke-WebRequest -Uri $MAIN_DOWNLOAD -UseBasicParsing -OutFile $MainDownloadZipName;
Invoke-WebRequest -Uri $Make_Download -UseBasicParsing -OutFile make.exe;
Write-Host 'Extracting archives...';
Expand-Archive $MainDownloadZipName -DestinationPath $Destination -Force;
Write-Host "$(Join-Path $Destination $XpackVersion) $Destination";
Move-Item -Force -Path $(Join-Path $(Join-Path $Destination $XpackVersion) *) -Destination $Destination;
Move-Item -Force -Path make.exe -Destination $(Join-Path $Destination $(Join-Path bin make.exe));
Remove-Item $(Join-Path $Destination $XpackVersion);
Remove-Item $MainDownloadZipName;

if (-NOT $NoPath)
{
    $PathPath = 'Registry::HKEY_CURRENT_USER\Environment';
    Write-Host 'Adding to user PATH environment variable...';
    $CurrentPATH = (Get-ItemProperty -Path $PathPath -Name 'PATH').Path;
    if ([string]::IsNullOrWhitespace($CurrentPATH)) { Write-Error 'Could not retrieve the user PATH'; Exit; }

    if ($CurrentPath.Contains($Destination.TrimEnd(('\', '/')))) # If the install dir is on the path, regardless of trailing slash or not
    {
        Write-Host '  It looks like this xpack installation is already in your user PATH, so it will not be edited.';
    }
    else
    {
        $NewPATH = "$CurrentPATH;$Destination/bin";
        Set-ItemProperty -Path $PathPath -Name 'PATH' -Value $NewPATH
        Write-Host '  You may need to restart your terminal before you can use xpack gcc.';
    }
}

Write-Host 'Finished!';