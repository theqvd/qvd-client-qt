

installVCRedist = function(name, arch, major_version, build, exe_file) {
	// name         : user friendly name for logs
	// major_version: the major version as it appears in the registry key
	// build        : build number of the redist, see below
	// exe          : installer executable
	//
	// To obtain the build number, run vc_redist.x64.exe to be shipped, look at the version number,
	// and take the last value. Eg, version '14.28.29325' => build 29325.



	console.log("Verifying Visual C " + name + " runtime for " + arch + ". We need version " + major_version + ", build " + build + ".");
	var runtime_key = "Runtimes";

	if (major_version < 11) {
		runtime_key = "VCRedist";
	}

	var reg_path = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\" + major_version + "\\VC\\" + runtime_key + "\\" + arch;
	console.log("Checking registry key: " + reg_path);


    var registryVC2017x64 = installer.execute("reg", new Array("QUERY", reg_path, "/v", "Installed"))[0];
    var doInstall = false;
    if (!registryVC2017x64) {
		console.log("Visual C runtime not found, will install.");
        doInstall = true;
    } else {
        var bld = installer.execute("reg", new Array("QUERY", reg_path, "/v", "Bld"))[0];

        var elements = bld.split(" ");

        bld = parseInt(elements[elements.length-1]);
		console.log("Visual C runtime build version is " + bld);
        if (bld < build) {
			console.log("Visual C runtime is too old, will install.");
            doInstall = true;
        } else {
			console.log("Visual C runtime is up to date.");
		}
    }

    if (doInstall) {
        //QMessageBox.information("vcRedist.install", "Install VS Redistributables", "The application requires Visual Studio 2017 Redistributables. Please follow the steps to install it now.", QMessageBox.OK);
        var dir = installer.value("TargetDir");
		var inst_exe = dir + "\\" + exe_file;

		// return value 1638 is unknown, but seems to mean a newer version is installed?
		// return value 3010 means a reboot is needed.
		// return value 5100 means a newer version is installed.
		console.log("Will run VC " + name + " redistributable install: " + inst_exe);
		component.addElevatedOperation("Execute", "{0,1638,3010,5100}", inst_exe, "/norestart", "/passive");

		// Remove the redist after the install, not needed anymore.
		component.addOperation("Delete", inst_exe);
    }
}

Component.prototype.createOperations = function() {
	component.createOperations();

	if ( systemInfo.productType === "windows" ) {
		console.log("Creating icon");

		component.addOperation("CreateShortcut",
		                       "@TargetDir@/QVD_Client.exe",
							   "@StartMenuDir@/QVD Client.lnk",
							   "workingDirectory=@TargetDir@",
							   "iconPath=@TargetDir@/QVD_Client.exe",
							   "description=QVD Client");

		component.addOperation("CreateShortcut",
		                       "@TargetDir@/QVD_Client.exe",
							   "@DesktopDir@/QVD Client.lnk",
							   "workingDirectory=@TargetDir@",
							   "iconPath=@TargetDir@/QVD_Client.exe",
							   "description=QVD Client");

		// This is needed for SSL, because the SSL libs are compiled with VC 2010.
		// See https://bugreports.qt.io/browse/QTBUG-78242
		installVCRedist("2010", "x64", "10.0", 40219, "vcredist_2010_x64.exe");

		// This is needed for our code.
		installVCRedist("2019", "x64", "14.0", 29325, "VC_redist.x64.exe");
	}
}

// This must exist even if empty:
// https://doc.qt.io/qtinstallerframework/scripting.html
function Component() {

}

