# Installer script based on TinyCC installer script by Macyler (CaiB) from https://raw.githubusercontent.com/cntools/Install-TCC
param (
	# The folder where Xpack will be installed to (default is %LOCALAPPDATA%\xpack-*)
	[Parameter(Position = 0, Mandatory = $false)]
	[string]$XpackInstallPath,
	
	# Set a custom temporary folder to keep ZIPs in while extracting (default is %TEMP%\ch32fun)
	[string]$TempFolder,

	# If enabled, will prevent modification of your PATH environment variable
	[switch]$NoPath,
	
	# If enabled, will not install our make
	[switch]$NoMake,

	# Skips all prompts and just installs, mainly intended for use if we need to re-run as admin.
	[switch]$SkipPrompts,
	
	# Does not delete the TempFolder at the end
	[switch]$NoClearTemp
);

$ErrorActionPreference = 'Stop';
Add-Type -Assembly 'System.IO.Compression.FileSystem';

$XpackVersion = '14.2.0-3';
$MakeVersion = '4.4.1';
$XpackEdition = 'riscv-none-elf-gcc';
$MakeEdition = 'without-guile';
$XpackArch = 'win32-x64';
$MakeArch = 'w32';

$XpackName = "xpack-$XpackEdition-$XpackVersion";
$MakeName = "make-$MakeVersion-$MakeEdition"
$XpackNameFull = "$XpackName-$XpackArch";
$MakeNameFull = "$MakeName-$MakeArch";
$XpackDownloadFilename = "$XpackNameFull.zip";
$MakeDownloadFilename = "$MakeNameFull-bin.zip";
$XpackDownloadUrl = "https://github.com/xpack-dev-tools/$XpackEdition-xpack/releases/download/v$XpackVersion/$XpackDownloadFilename";
$MakeDownloadUrl = "https://sourceforge.net/projects/ezwinports/files/$MakeDownloadFilename/download";

if ([string]::IsNullOrEmpty($XpackInstallPath)) { $XpackInstallPath = Join-Path $ENV:LOCALAPPDATA $XpackName; }
if ([string]::IsNullOrEmpty($TempFolder)) { $TempFolder = $ENV:TEMP; }
$TempFolder = Join-Path $TempFolder 'ch32fun';
$XpackDownloadFilePath = Join-Path $TempFolder $XpackDownloadFilename;
$MakeDownloadFilePath = Join-Path $TempFolder $MakeDownloadFilename;
$XpackBinPath = Join-Path $XpackInstallPath 'bin';
$MakeTempPath = Join-Path $TempFolder make;
$MakeTempExePath = Join-Path $(Join-Path $MakeTempPath bin) 'make.exe';

Write-Host "$XpackName will be downloaded to $XpackDownloadFilePath from $XpackDownloadUrl, then installed to $XpackInstallPath`n";
if (-NOT $NoMake) { Write-Host "$MakeName will be downloaded to $MakeDownloadFilePath from $MakeDownloadUrl, then installed to $XpackBinPath`n"; }
if ($NoPath) {
	Write-Host "$XpackBinPath will NOT be added to your user PATH environment variable.`n";
} else {
	Write-Host "$XpackBinPath will be added to your user PATH environment variable.`n";
}
if (-NOT $SkipPrompts) {
	[string]$Answer = Read-Host -Prompt "Is this correct? (y/n)";
	if ($Answer -NE 'y') { Exit; }
}

[void](New-Item -Path $TempFolder -ItemType Directory -Force);

Write-Host "(Showing progress makes it download very slowly for some reason, so we won't show progress)";
$ProgressPreference = 'SilentlyContinue';

Write-Host "Downloading $XpackDownloadFilename...";
Invoke-WebRequest -UseBasicParsing -Uri $XpackDownloadUrl -OutFile $XpackDownloadFilePath;
if (-NOT $NoMake) {
	Write-Host "Downloading $MakeDownloadFilename...";
	Invoke-WebRequest -UseBasicParsing -UserAgent 'Wget' -Uri $MakeDownloadUrl -OutFile $MakeDownloadFilePath;
}
Write-Host "Extracting $XpackDownloadFilename...";
Expand-Archive -Force $XpackDownloadFilePath -DestinationPath $XpackInstallPath;
if (-NOT $NoMake) {
	Write-Host "Extracting $MakeDownloadFilename...";
	Expand-Archive -Force $MakeDownloadFilePath -DestinationPath $MakeTempPath;
}

Move-Item -Force -Path $(Join-Path $(Join-Path $XpackInstallPath $XpackName) *) -Destination $XpackInstallPath;
Remove-Item $(Join-Path $XpackInstallPath $XpackName);

if (-NOT $NoMake) {
	Write-Host "Moving $MakeTempExePath to $XpackBinPath...";
	Move-Item -Force -Path $MakeTempExePath -Destination $XpackBinPath;
}

if (-NOT $NoClearTemp) {
	Write-Host "Deleting $TempFolder...";
	Remove-Item $TempFolder -Force -Recurse;
}

if (-NOT $NoPath)
{
	$PathPath = 'Registry::HKEY_CURRENT_USER\Environment';
	Write-Host 'Adding to user PATH environment variable...';
	$CurrentPATH = (Get-ItemProperty -Path $PathPath -Name 'PATH').Path;
	if ([string]::IsNullOrWhitespace($CurrentPATH)) {
		Write-Error 'Could not retrieve the user PATH';
		Exit;
	}
	if ($CurrentPath.Contains($XpackBinPath.TrimEnd(('\', '/')))) { # If the install dir is on the path, regardless of trailing slash or not
		Write-Host '  It looks like this xpack installation is already in your user PATH, so it will not be edited.';
	} else {
		$NewPATH = "$CurrentPATH;$XpackBinPath";
		Set-ItemProperty -Path $PathPath -Name 'PATH' -Value $NewPATH;
		Write-Host '  You may need to restart your terminal before you can use xpack gcc.';
	}
}

Write-Host 'Finished!';