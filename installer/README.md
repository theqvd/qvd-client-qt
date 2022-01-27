# Windows installer build scripts
 
## Requirements
 
* Qt 5.15
* PowerShell 7
* CMake (present in PATH)
* Ninja (present in PATH)
 
## Optional
 
### Auto-upload script.
 
Create a script in `My Documents\installer_uploader.ps1`
 
It has to take a filename as an argument, and return a list of URLs where it uploaded it. Pattern:
 
```
$installer_file = $args[0]
$filename = [System.IO.Path]::GetFileName($installer_file)
$urls = @( )

Write-Host "Uploading: $installer_file"

# (Upload the file somewhere)

$urls += "https://myserver.com/qvd-client-qt/$filename"

return $urls
 ```


## Making a self-signed code certificate

If you have no certificate, run `.\makecert.ps1` to make one. If the uploader script exists, the certificate will be uploaded with it. 

## Importing a code certificate

Run `.\importcert.ps1 certificate.crt`

## Building

Run `.\build.ps1`

Accepted arguments:

 * -Verbose: Show more output
 * -NoUpload: Don't run the upload script


