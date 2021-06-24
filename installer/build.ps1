
param (
	[switch]$Verbose=$false,
	[switch]$NoUpload=$false
)


$ErrorActionPreference = "Stop"
$VCVarsAll = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
$FilesPath = "C:\Program Files (x86)\QVD Client"
$ExtFiles  = "${PSScriptRoot}\..\external"
$TimestampServer = "http://timestamp.sectigo.com"
$CertificateThumbprint = "4EC4BC69CAF66CFFB8EA1245E12C4C4291A887DB"

$Certificate = Get-ChildItem cert:\CurrentUser\My -CodeSigningCert | Where-Object { $_.Thumbprint -eq "$CertificateThumbprint" }


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

function Sign {
	param([string]$Path)

	$relpath = Get-Item $Path | Resolve-Path -Relative

	Write-Host -NoNewLine "Signing $relpath... "
	$sign_info = Get-AuthenticodeSignature $Path

	if ( $sign_info.Status -ne "Valid" ) {
		#$sign_info = Set-AuthenticodeSignature $Path -Certificate (Get-ChildItem cert:\CurrentUser\My -CodeSigningCert) -HashAlgorithm SHA256 -TimestampServer $TimestampServer
		$sign_info = Set-AuthenticodeSignature $Path -Certificate $Certificate -HashAlgorithm SHA256 -TimestampServer $TimestampServer

		if ( $sign_info.Status -eq "Valid" ) {
			Write-Host -ForegroundColor green "Done."
		} else {
			Write-Host -ForegroundColor red "Error! Signature status is $sign_info.Status"
		}
	} else {
		Write-Host -ForegroundColor cyan "Already signed."
	}

	return $sign_info
}

function Header {
	param([string]$Label)

	$W = (Get-Host).UI.RawUI.WindowSize.Width
	$HDR = " " * $W
	$Padding = " " * ($W/2 - $Label.length / 2)
	$Padding2 = " " * ($W - $Padding.length - $Label.length)

	Write-Host ""
	Write-Host -ForegroundColor white -BackgroundColor blue $HDR
	Write-Host -ForegroundColor white -BackgroundColor blue "${Padding}${Label}${Padding2}"
	Write-Host -ForegroundColor white -BackgroundColor blue $HDR

	# Sometimes (randomly) the next line gets the background color. Clear it.
	Write-Host -NoNewLine "$HDR`r"
}


$QT_VER="5.15.2"
$QT_DIR="C:\Qt"
$use_mingw = 0
$MAKETOOL_ARGS = ""

if ( $use_mingw ) {
	$QT_COMPILER="mingw81_64"
	$COMPILER="mingw810_64"
	$SPEC="win32-g++"
	$MAKETOOL="mingw32-make"
} else {
	$QT_COMPILER="msvc2019_64"
	$COMPILER="mingw810_64"
	$SPEC="win32-msvc"
	$MAKETOOL="ninja"

	if ( $Verbose ) {
		$MAKETOOL_ARGS="-v"
	}
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

if ( $git_ver[0] -eq "v" ) {
	$git_ver = $git_ver.substring(1)
}

# git describe generates stuff like:
# 4.3.0-2-g4efb2d5
#
# 4.3.0 comes from the tag, v4.3.0
# 2 is the number of commits since the v4.3.0 tag
# g4efb2d5 is the git commit 4efb2d5

$ver_parts = $git_ver.Split(".")
$rev_parts = $ver_parts[2].Split("-")

$Env:QVD_VERSION_MAJOR    = $ver_parts[0]
$Env:QVD_VERSION_MINOR    = $ver_parts[1]
$Env:QVD_VERSION_REVISION = $rev_parts[0]
$Env:QVD_VERSION_FULL     = $git_ver

if ( ! $Env:BUILD_NUMBER ) {
	Write-Host "BUILD_NUMBER variable not set, using build counter"

	$build_file = "${PSScriptRoot}/build-num.txt"

	if ( ! (Test-Path "$build_file") ) {
		Write-Host "No build counter found, creating"
		Set-Content -Path "$build_file" -Value '1'
	}

	$build = [int](Get-Content -Path "$build_file")
	$build++

	Set-Content -Path "$build_file" -Value "$build"

	$Env:QVD_BUILD = $build
} else {
	# Get the build number from Hudson
	$Env:QVD_BUILD = $Env:BUILD_NUMBER
}

Header "Starting build"

Write-Host -ForegroundColor blue -NoNewLine "Version  : "
Write-Host "$git_ver ($Env:QVD_VERSION_MAJOR, $Env:QVD_VERSION_MINOR, $Env:QVD_VERSION_REVISION)"

Write-Host -ForegroundColor blue -NoNewLine "Commit   : "
Write-Host "$git_commit"

Write-Host -ForegroundColor blue -NoNewLine "Build    : "
Write-Host "$Env:QVD_BUILD"


$subj=$Certificate.Subject.Split(",")[0].Replace("CN=","").Replace('"','')
$issuer=$Certificate.Issuer.Split(",")[0].Replace("CN=","").Replace('"','')
$valid=$Certificate.NotAfter

Write-Host -ForegroundColor blue -NoNewLine "Cert     : "
Write-Host "$subj (Valid until $valid)"
Write-Host -ForegroundColor blue -NoNewLine "Issued by: "
Write-Host "$issuer"
Write-Host ""

$build_dir = New-TemporaryDirectory
Set-Location -Path "$build_dir"

cmake "${PSScriptRoot}\..\qtclient" -G Ninja

if ( $LastExitCode -gt 0 ) {
	throw "cmake failed with status $LastExitCode !"
}

iex "$MAKETOOL $MAKETOOL_ARGS"
if ( $LastExitCode -gt 0 ) {
	throw "$MAKETOOL failed with status $LastExitCode !"
}


Header "Copying data"

Set-Location -Path "$PSScriptRoot"

if ( Test-Path "packages\com.qindel.qvd\data" ) {
	Remove-Item "packages\com.qindel.qvd\data" -Recurse -Force
}


$junk = New-Item -Path "packages\com.qindel.qvd"      -Name "data"    -ItemType Directory
$junk = New-Item -Path "packages\com.qindel.qvd\data" -Name "bin"     -ItemType Directory
$junk = New-Item -Path "packages\com.qindel.qvd\data" -Name "scripts" -ItemType Directory


$data = "packages\com.qindel.qvd\data"

Write-Host "Copying build files..."
Copy-Item -Path "..\LICENSE"                             -Destination "$data\LICENSE.txt"
Copy-Item -Path "$build_dir\gui\*.exe"                   -Destination "$data"
Copy-Item -Path "$build_dir\libqvdclient\*.dll"          -Destination "$data"

Write-Host "Copying dependencies..."
Copy-Item -Path "$FilesPath\pulseaudio"                  -Destination "$data" -Recurse
Copy-Item -Path "$FilesPath\vcxsrv"                      -Destination "$data" -Recurse
Copy-Item -Path "$ExtFiles\nx\*"                         -Destination "$data\bin\" -Recurse
Copy-Item -Path "$FilesPath\win-sftp-server.exe"         -Destination "$data"
Copy-Item -Path "$FilesPath\redist\*.exe"                -Destination "$data"
Copy-Item -Path "$SSL_BIN_PATH\libcrypto*"               -Destination "$data"
Copy-Item -Path "$SSL_BIN_PATH\libssl*"                  -Destination "$data"
Copy-Item -Path "install_scripts\*"                      -Destination "$data\scripts\"

$deploy_args = "--verbose=0"
if ( $Verbose ) {
	$deploy_args = "--verbose=2"
}

Write-Host "Deploying Qt..."
windeployqt $deploy_args "$data"
if ( $LastExitCode -gt 0 ) {
	throw "$windeployqt failed with status $LastExitCode !"
}


Header "Signing"

$binaries = Get-ChildItem -Path $data -Filter '*.exe' -Recurse -ErrorAction SilentlyContinue -Force
foreach ($bin in $binaries) {
	$signature = Sign $bin.FullName
}



Header "Generating installer"

$creator_args = ""
if ($Verbose) {
	$creator_args = "-v"
}

Write-Host "Generating installer..."
binarycreator $creator_args -c config\config.xml -p packages qvd-client-installer-${git_ver}-${Env:QVD_BUILD}
if ( $LastExitCode -gt 0 ) {
	throw "binarycreator failed with status $LastExitCode !"
}

$installer_file = "$PSScriptRoot\qvd-client-installer-${git_ver}-${Env:QVD_BUILD}.exe"

$installer_signature = Sign $installer_file

$installer_hash = (Get-FileHash $installer_file).Hash
$installer_mb = [math]::round( (Get-Item $installer_file).Length / 1MB, 2 )

# This part can run an uploader script to automatically upload the installer
# wherever appropriate.
#
# The script must be a PowerShell script.
# It takes the path to the generated installer as an argument.
# It returns an array of URLs to where the uploaded installers are.
#
if (! $NoUpload ) {
	$docs = [Environment]::GetFolderPath("MyDocuments")
	$uploader_script = "$docs/installer_uploader.ps1"
	$urls = @( )

	if ( Test-Path "$uploader_script" ) {
		Header "Uploading"

		$urls = &"$uploader_script" "$installer_file"
	} else {
		Write-Host "No uploader script found, looked in $uploader_script"
	}
}

Header "Build completed"
#Write-Host ""
#Write-Host -ForegroundColor white "*********************************************************************************"
#Write-Host -ForegroundColor blue  "                               Build completed"
Write-Host -ForegroundColor blue -NoNewLine "Installer: "
Write-Host "$installer_file"

Write-Host -ForegroundColor blue -NoNewLine "Size     : "
Write-Host "$installer_mb MiB"

Write-Host -ForegroundColor blue -NoNewLine "SHA256   : "
Write-Host "$installer_hash"

Write-Host -ForegroundColor blue -NoNewLine "Signed by: "
Write-Host -NoNewLine $installer_signature.SignerCertificate.Issuer

if ( $installer_signature.SignerCertificate.Issuer -eq $installer_signature.SignerCertificate.Subject ) {
	Write-Host -ForegroundColor red " [SELF-SIGNED]"
} else {
	Write-Host -ForegroundColor green " [Official]"
}

Write-Host -ForegroundColor blue -NoNewLine "Signature: "
Write-Host $installer_signature.Status



Foreach ($url in $urls) {
	Write-Host -ForegroundColor blue -NoNewLine "URL      : "
	Write-Host "$url"
}

Write-Host ""

