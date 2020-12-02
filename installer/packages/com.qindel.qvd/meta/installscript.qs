Component.prototype.installVCRedist = function() {
	// To obtain this number, run vc_redist.x64.exe to be shipped, look at the version number,
	// and take the last value. Eg, version '14.28.29325' => build 29325.
	
	var VCREDIST_BUILD = 29325;

	console.log("Verifying Visual C runtime. We need at least build " + VCREDIST_BUILD + ".");
	
    var registryVC2017x64 = installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", "/v", "Installed"))[0];
    var doInstall = false;
    if (!registryVC2017x64) {
		console.log("Visual C runtime not found, will install.");
        doInstall = true;
    } else {
        var bld = installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", "/v", "Bld"))[0];

        var elements = bld.split(" ");

        bld = parseInt(elements[elements.length-1]);
		console.log("Visual C runtime build version is " + bld);
        if (bld < VCREDIST_BUILD) {
			console.log("Visual C runtime is too old, will install.");
            doInstall = true;
        } else {
			console.log("Visual C runtime is up to date.");
		}
    }

    if (doInstall) {
        QMessageBox.information("vcRedist.install", "Install VS Redistributables", "The application requires Visual Studio 2017 Redistributables. Please follow the steps to install it now.", QMessageBox.OK);
        var dir = installer.value("TargetDir");
		console.log("Running VC install");
        installer.execute(dir + "/VC_redist.x64.exe", "/norestart", "/passive");
    }
}


function Component() {
	if (systemInfo.productType === "windows") {
		installer.installationFinished.connect(this, Component.prototype.installVCRedist);
	}
}