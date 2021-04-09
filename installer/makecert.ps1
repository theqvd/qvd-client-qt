param (
	[switch]$NoUpload=$false
)



$docs = [Environment]::GetFolderPath("MyDocuments")
$cert = (Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert)[0]

if (!$cert ) {
	New-SelfSignedCertificate -DnsName qvd@qindel.com -Type CodeSigning -CertStoreLocation cert:\CurrentUser\My
	$cert = (Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert)[0]
}
	
Export-Certificate -Cert $cert -FilePath $docs\code_signing.crt
Import-Certificate -FilePath $docs\code_signing.crt -Cert Cert:\CurrentUser\TrustedPublisher
Import-Certificate -FilePath $docs\code_signing.crt -Cert Cert:\CurrentUser\Root

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
		Write-Host "Running uploader script"
		
		$urls = &"$uploader_script" $docs\code_signing.crt
	} else {
		Write-Host "No uploader script found, looked in $uploader_script"
	}
	
	
	Foreach ($url in $urls) {
		Write-Host -ForegroundColor blue -NoNewLine "URL      : "
		Write-Host "$url"
	}
}

