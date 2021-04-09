$cert = $args[0]

Import-Certificate -FilePath $cert -Cert Cert:\CurrentUser\TrustedPublisher
Import-Certificate -FilePath $cert -Cert Cert:\CurrentUser\Root
