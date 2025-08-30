using namespace System.IO;
using namespace System.IO.Compression.FileSystem;

[CmdletBinding(DefaultParameterSetName = 'DestPreset')]
param (
	# Uses one of the preset locations to install to
	[Parameter(ParameterSetName = 'DestPreset', Position = 0)]
	[ValidateSet('User', 'System')]
	[string]$Destination = 'User',

	# If a preset location is not used, then the user must specify a path to install to
	[Parameter(ParameterSetName = 'DestPath', Position = 0, Mandatory = $true)]
	[string]$Path,

	# If enabled, will prevent modification of your PATH environment variable
	[switch]$NoPath,
	
	# If enabled, will not install our make
	[switch]$NoMake,

	# Skips all prompts and just installs, mainly intended for use if we need to re-run as admin.
	[switch]$SkipPrompts
);

$ErrorActionPreference = 'Stop';

[string] $XpackVersion = '14.2.0-3';
[string] $XpackEdition = 'riscv-none-elf-gcc';
[string] $XpackArch = 'win32-x64';
[string] $XpackNameEd = "xpack-$XpackEdition";
[string] $XpackNameEdVer = "$XpackNameEd-$XpackVersion";
[string] $XpackNameEdVerArch = "$XpackNameEdVer-$XpackArch"
[string] $XpackDownloadFilename = "$XpackNameEdVerArch.zip";
[string] $XpackDownloadURL = "https://github.com/xpack-dev-tools/$XpackEdition-xpack/releases/download/v$XpackVersion/$XpackDownloadFilename";

[string] $MakeVersion = '4.4.1';
[string] $MakeEdition = 'without-guile';
[string] $MakeArch = 'w32';
[string] $MakeName = "make-$MakeVersion-$MakeEdition"
[string] $MakeNameFull = "$MakeName-$MakeArch";
[string] $MakeDownloadFilename = "$MakeNameFull-bin.zip";
[string] $MakeDownloadURL = "https://sourceforge.net/projects/ezwinports/files/$MakeDownloadFilename/download";

[EnvironmentVariableTarget] $PathScope = [EnvironmentVariableTarget]::User;
[string] $TempFolder = [Path]::GetTempPath();
[bool] $NoClearTemp = $false;

if ($PSCmdlet.ParameterSetName -EQ 'DestPreset')
{
	if ($Destination -EQ 'User') { $Path = Join-Path $ENV:LocalAppData $XpackNameEd; }
	elseif ($Destination -EQ 'System')
	{
		$Path = Join-Path $ENV:ProgramFiles $XpackNameEd;
		$PathScope = [EnvironmentVariableTarget]::Machine;
	}
	else { throw 'Unknown Preset Destination'; }
}
if ([string]::IsNullOrEmpty($Path)) { throw 'Destination path must be provided'; }

[string] $TempFolder = Join-Path $TempFolder 'ch32fun';
[string] $XpackDownloadFilePath = Join-Path $TempFolder $XpackDownloadFilename;
[string] $XpackPath = Join-Path $Path $XpackVersion;
[string] $XpackBinPath = Join-Path $XpackPath 'bin';
[string] $MakeDownloadFilePath = Join-Path $TempFolder $MakeDownloadFilename;

Write-Host "${XpackNameEdVer}:";
Write-Host "  will be downloaded from " -NoNewline -ForegroundColor 'DarkGray';
Write-Host $XpackDownloadURL -ForegroundColor 'DarkGreen';
Write-Host "  to " -NoNewline -ForegroundColor 'DarkGray';
Write-Host $XpackDownloadFilePath -ForegroundColor 'DarkGreen';
Write-Host "  then installed to " -NoNewline -ForegroundColor 'DarkGray';
Write-Host $XpackPath -ForegroundColor 'DarkGreen';
Write-Host;
if (-NOT $NoMake)
{
	Write-Host "${MakeName}:"
	Write-Host "  will be downloaded from " -NoNewline -ForegroundColor 'DarkGray';
	Write-Host $MakeDownloadURL -ForegroundColor 'DarkGreen';
	Write-Host "  to " -NoNewline -ForegroundColor 'DarkGray';
	Write-Host $MakeDownloadFilePath -ForegroundColor 'DarkGreen';
	Write-Host "  then installed to " -NoNewline -ForegroundColor 'DarkGray';
	Write-Host $XpackBinPath -ForegroundColor 'DarkGreen';
	Write-Host;
}
if ($NoPath) { Write-Host "Your environment variables will not be edited.`n"; }
else
{
	Write-Host "xpack will be added to your " -NoNewline;
	Write-Host $PathScope.ToString().ToLower() -ForegroundColor 'DarkGreen' -NoNewline; 
	Write-Host " PATH environment variable.`n";
}
if (-NOT $SkipPrompts)
{
	[string] $Answer = Read-Host -Prompt "Is this correct? (y/n)";
	if ($Answer -NE 'y') { Exit; }
}

try
{ # Check if we need elevation
    [bool] $IsAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] 'Administrator');
    if ((-NOT $NoPath) -AND ($PathScope -EQ [EnvironmentVariableTarget]::Machine) -AND (-NOT $IsAdmin)) { throw; } # If we want to set the system PATH variable, we need to elevate
    if (-NOT (Test-Path $Path)) { New-Item -ItemType Directory -Path $Path -ErrorAction SilentlyContinue -ErrorVariable PermissionError | Out-Null; }
    if ($PermissionError) { throw; }
    [string] $TestFile = Join-Path $Path 'TestingFile.txt';
    Set-Content -Path $TestFile -Value 'Checking permissions.' -ErrorAction SilentlyContinue -ErrorVariable PermissionError;
    if ($PermissionError) { throw; }
    Remove-Item -Path $TestFile -ErrorAction SilentlyContinue -ErrorVariable PermissionError;
    if ($PermissionError) { throw; }
}
catch
{
    Write-Host 'Administrator permissions are required, requesting elevation...';
    if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] 'Administrator'))
    {
        if ([int](Get-CimInstance -Class Win32_OperatingSystem | Select-Object -ExpandProperty BuildNumber) -GE 6000)
        {
            $CommandLine = "-File `"$($MyInvocation.MyCommand.Path)`" -SkipPrompts";
			if ($PSCmdlet.ParameterSetName -EQ 'DestPreset') { $CommandLine += " -Destination `"$Destination`""; }
			else { $CommandLine += " -Path `"$Path`""; }
            if ($NoPath) { $CommandLine += ' -NoPath'; }
			if ($NoMake) { $CommandLine += ' -NoMake'; }
            try { Start-Process -FilePath 'PowerShell.exe' -Verb RunAs -ArgumentList $CommandLine; }
            catch { Write-Error 'Failed to elevate to administrator.'; Exit; }
        }
    }
    else { Write-Error 'Could not access folder, but am already administrator!'; Exit; }
    if (-NOT $NoPath) { Write-Host 'You may need to restart your terminal before changes apply.'; }
    Exit;
}

[void](New-Item -Path $TempFolder -ItemType Directory -Force);
[void](New-Item -Path $Path -ItemType Directory -Force);

if ($PSVersionTable.PSVersion.Major -LT 7) { $ProgressPreference = 'SilentlyContinue'; } # Showing progress makes it download very slowly on old PowerShell https://github.com/PowerShell/PowerShell/issues/2138

Write-Host "Downloading $XpackDownloadFilename" -NoNewline;
try { Write-Host $(' ({0:F2} MB)...' -F ([long]::Parse((Invoke-WebRequest -UseBasicParsing -Uri $XpackDownloadURL -Method Head).Headers['Content-Length']) / (1024.0 * 1024.0))); }
catch { Write-Host '...'; }
Invoke-WebRequest -UseBasicParsing -Uri $XpackDownloadURL -OutFile $XpackDownloadFilePath;

if (-NOT $NoMake)
{
	Write-Host "Downloading $MakeDownloadFilename...";
	Invoke-WebRequest -UseBasicParsing -UserAgent 'Wget' -Uri $MakeDownloadURL -OutFile $MakeDownloadFilePath;
}

Write-Host "Extracting $XpackDownloadFilename...";
[string] $XpackTempPath = Join-Path $TempFolder 'xpath';
if (Test-Path $XpackTempPath) { Remove-Item -Recurse -Force $XpackTempPath; }
Expand-Archive $XpackDownloadFilePath -DestinationPath $XpackTempPath;
if (-NOT (Test-Path $XpackPath)) { [void](New-Item -Path $XpackPath -ItemType Directory -Force); }
Copy-Item -Path $(Join-Path $(Join-Path $XpackTempPath $XpackNameEdVer) '*') -Destination $XpackPath -Recurse -Force;

if (-NOT $NoMake)
{
	[string] $MakeTempPath = Join-Path $TempFolder 'make';
	Write-Host "Extracting $MakeDownloadFilename...";
	Expand-Archive -Force $MakeDownloadFilePath -DestinationPath $MakeTempPath;
	Move-Item -Force -Path $(Join-Path $MakeTempPath 'bin/make.exe') -Destination $XpackBinPath;
}

if (-NOT $NoClearTemp)
{
	Write-Host "Deleting $TempFolder...";
	Remove-Item $TempFolder -Force -Recurse;
}

if (-NOT $NoPath)
{
	Write-Host "Adding to $PathScope PATH environment variable...";
	$CurrentPATH = [Environment]::GetEnvironmentVariable('PATH', $PathScope);
	if ([string]::IsNullOrWhitespace($CurrentPATH)) { Write-Error 'Could not retrieve the current PATH, not editing'; Exit; }

	if ($CurrentPATH.Contains($XpackBinPath.TrimEnd(('\', '/')))) # If the install dir is on the path, regardless of trailing slash or not
	{
		Write-Host '  It looks like this xpack installation is already in your user PATH, so it will not be edited.';
	}
	else {
		$NewPATH = "$CurrentPATH;$XpackBinPath";
		[Environment]::SetEnvironmentVariable('PATH', $NewPATH, $PathScope);
		Write-Host '  You may need to restart your terminal before you can use xpack gcc.';
	}
	# Detect GitHub Actions runner and export for future steps
    if ($env:GITHUB_ACTIONS -eq 'true') {
        Write-Host "Detected GitHub Actions runner. Prepending xPack bin to PATH for workflow steps..."
        Add-Content -Path $env:GITHUB_ENV -Value "PATH=$XpackBinPath`:$env:PATH"
    }
}

Write-Host 'Finished!';