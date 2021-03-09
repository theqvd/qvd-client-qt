$ErrorActionPreference = "Stop"
$VCVarsAll = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
$FilesPath = "C:\Program Files (x86)\QVD Client"


function Invoke-BatchFile
{
   param([string]$Path, [string]$Parameters)  

   $tempFile = [IO.Path]::GetTempFileName()  

	Write-Host "Path is $Path"
	
   ## Store the output of cmd.exe.  We also ask cmd.exe to output   
   ## the environment table after the batch file completes  
   cmd.exe /c " `"$Path`" $Parameters && set > `"$tempFile`" " 

   ## Go through the environment variables in the temp file.  
   ## For each of them, set the variable in our local environment.  
   Get-Content $tempFile | Foreach-Object {   
       if ($_ -match "^(.*?)=(.*)$")  
       { 
           Set-Content "env:\$($matches[1])" $matches[2]  
       } 
   }  

   Remove-Item $tempFile
}

function New-TemporaryDirectory {
  $parent = [System.IO.Path]::GetTempPath()
  do {
    $name = [System.IO.Path]::GetRandomFileName()
    $item = New-Item -Path $parent -Name $name -ItemType "directory" -ErrorAction SilentlyContinue
  } while (-not $item)
  return $Item.FullName
}



$QT_VER="5.15.2"
$QT_DIR="C:\Qt"
$use_mingw = 0

if ( $use_mingw ) {
	$QT_COMPILER="mingw81_64"
	$COMPILER="mingw810_64"
	$SPEC="win32-g++"
	$MAKETOOL="mingw32-make"
} else {
	$QT_COMPILER="msvc2019_64"
	$COMPILER="mingw810_64"
	$SPEC="win32-msvc"
	$MAKETOOL="jom"
}

if ( ! $Env:VCINSTALLDIR ) {
	# Import VC vars if not already done.
	Invoke-BatchFile "$VCVarsAll" "x64"
}

$QT_BIN_PATH="$QT_DIR\$QT_VER\$QT_COMPILER\bin"
$COMPILER_BIN_PATH="$QT_DIR\Qt\Tools\COMPILER\bin"
$QT_INSTALLER_BIN_PATH="$QT_DIR\Tools\QtInstallerFramework\4.0\bin"
$SSL_BIN_PATH="$QT_DIR\Tools\OpenSSL\Win_x64\bin"


$env:PATH="$QT_BIN_PATH;$COMPILER_BIN_PATH;$QT_INSTALLER_BIN_PATH;$QT_DIR\Tools\QtCreator\bin;$env:PATH"


$git_ver = git describe HEAD --tags
$git_commit = git rev-parse HEAD

$git_ver = $git_ver -replace '[a-zA-Z]'

$ver_parts = $git_ver.Split(".")
$Env:QVD_VERSION_MAJOR    = $ver_parts[0]
$Env:QVD_VERSION_MINOR    = $ver_parts[1]
$Env:QVD_VERSION_REVISION = $ver_parts[2]

if ( ! $Env:BUILD_NUMBER ) {
	Write-Host "QVD_BUILD variable not set, using build counter"

	$build_file = "${PSScriptRoot}/build-num.txt"

	if ( ! (Test-Path "$build_file") ) {
		Write-Host "No build counter found, creating"
		Set-Content -Path "$build_file" -Value '1'
	}

	$build = [int](Get-Content -Path "$build_file")
	Write-Host "Read $build"
	$build++
	Write-Host "inc: $build"

	Set-Content -Path "$build_file" -Value "$build"

	$Env:QVD_BUILD = $build
} else {
	# Get the build number from Hudson
	$Env:QVD_BUILD = $Env:BUILD_NUMBER
}

Write-Host ""
Write-Host "*******************************************************************"
Write-Host "                           Starting build"
Write-Host "Version  : $git_ver ($Env:QVD_VERSION_MAJOR, $Env:QVD_VERSION_MINOR, $Env:QVD_VERSION_REVISION)"
Write-Host "Commit   : $git_commit"
Write-Host "Build    : $Env:QVD_BUILD"
Write-Host "*******************************************************************"
Write-Host ""




$build_dir = New-TemporaryDirectory
Set-Location -Path "$build_dir"


qmake "${PSScriptRoot}\..\qtclient\QVD_Client.pro" -spec $SPEC 
if ( $LastExitCode -gt 0 ) {
	throw "qmake failed with status $LastExitCode !"
}

iex $MAKETOOL
if ( $LastExitCode -gt 0 ) {
	throw "$MAKETOOL failed with status $LastExitCode !"
}


Set-Location -Path "$PSScriptRoot"

if ( Test-Path "packages\com.qindel.qvd\data" ) {
	Remove-Item "packages\com.qindel.qvd\data" -Recurse -Force
}


mkdir packages\com.qindel.qvd\data

$data = "packages\com.qindel.qvd\data"

Copy-Item -Path "..\LICENSE"                             -Destination "$data\LICENSE.txt"
Copy-Item -Path "$build_dir\gui\release\*.exe"           -Destination "$data"
Copy-Item -Path "$build_dir\libqvdclient\release\*.dll"  -Destination "$data"
Copy-Item -Path "$FilesPath\pulseaudio"                  -Destination "$data" -Recurse
Copy-Item -Path "$FilesPath\vcxsrv"                      -Destination "$data" -Recurse
Copy-Item -Path "$FilesPath\bin"                         -Destination "$data" -Recurse
Copy-Item -Path "$FilesPath\win-sftp-server.exe"         -Destination "$data"
Copy-Item -Path "$FilesPath\redist\*.exe"                -Destination "$data"
Copy-Item -Path "$SSL_BIN_PATH\libcrypto*"               -Destination "$data"
Copy-Item -Path "$SSL_BIN_PATH\libssl*"               -Destination "$data"


windeployqt "$data"
binarycreator -v -c config\config.xml -p packages qvd-client-installer-${git_ver}-${Env:QVD_BUILD}

$installer_file = "$PSScriptRoot\qvd-client-installer-${git_ver}-${Env:QVD_BUILD}.exe"
$installer_hash = (Get-FileHash $installer_file).Hash
$installer_mb = [math]::round( (Get-Item $installer_file).Length / 1MB, 2 )

Write-Host ""
Write-Host "*************************************************************************"
Write-Host "Installer: $installer_file"
Write-Host "Size     : $installer_mb MiB"
Write-Host "SHA256   : $installer_hash"
Write-Host "*************************************************************************"



### This part can run an uploader script to automatically upload the installer
### wherever appropriate.
$docs = [Environment]::GetFolderPath("MyDocuments")
$uploader_script = "$docs/installer_uploader.ps1"

if ( Test-Path "$uploader_script" ) {
	Write-Host "Running uploader script"
	
	Start-Process powershell -Wait -Argument "$uploader_script $installer_file" -NoNewWindow
} else {
	Write-Host "No uploader script found, looked in $uploader_script"
}



