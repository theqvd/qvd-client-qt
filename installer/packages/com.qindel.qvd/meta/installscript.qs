
var targetDirectoryPage = null;



setupFirewall = function() {

	netsh = "netsh.exe"
	vcxsrv = "@TargetDir@\\VcxSrv\\VcxSrv.exe"

	console.log("Setting up firewall rules");

	// QVD doesn't require any remote access to VcxSrv, but it doesn't seem to have an option
	// to just listen on localhost. So to avoid it asking the user, we just block it.

	component.addElevatedOperation("Execute", "{0}", netsh, "advfirewall", "firewall", "add", "rule",
                                   "name=QVD VcxSrv - No TCP",
								   "dir=in",
								   "protocol=tcp",
								   "action=block",
								   "program=" + vcxsrv,
								   "description=QVD only requires local access",
								   "UNDOEXECUTE",
								   netsh, "advfirewall", "firewall", "delete", "rule", "name=QVD VcxSrv - No TCP");

	component.addElevatedOperation("Execute", "{0}", netsh, "advfirewall", "firewall", "add", "rule",
                                   "name=QVD VcxSrv - No UDP",
								   "dir=in",
								   "protocol=udp",
								   "action=block",
								   "program=" + vcxsrv,
								   "description=QVD only requires local access",
								   "UNDOEXECUTE",
								   netsh, "advfirewall", "firewall", "delete", "rule", "name=QVD VcxSrv - No UDP"
								   );



}

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

installMSI = function(msi_file) {
	var dir = installer.value("TargetDir");
	var inst_msi = dir + "\\" + msi_file;
	
	console.log("Will install MSI " + msi_file + " redistributable install: " + inst_msi);

	component.addElevatedOperation("Execute", "{0,1638,3010,5100}", "msi_exec", "/qn", "/i", inst_msi);

	component.addOperation("Delete", inst_msi);
	
	
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

		// This is for USBIP
		installMSI("usbipd-win_3.0.0.msi");
		
		// Configure the firewall
		setupFirewall();
	}
}

// This must exist even if empty:
// https://doc.qt.io/qtinstallerframework/scripting.html
function Component() {
    installer.gainAdminRights();
    component.loaded.connect(this, this.installerLoaded);
}


//////////////////////////////////



Component.prototype.installerLoaded = function()
{
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory);

    targetDirectoryPage = gui.pageWidgetByObjectName("DynamicTargetWidget");
    targetDirectoryPage.windowTitle = "Choose Installation Directory";
    targetDirectoryPage.description.setText("Please select where the QVD Client will be installed:");
    targetDirectoryPage.targetDirectory.textChanged.connect(this, this.targetDirectoryChanged);
    targetDirectoryPage.targetDirectory.setText(installer.value("TargetDir"));
    targetDirectoryPage.targetChooser.released.connect(this, this.targetChooserClicked);

    gui.pageById(QInstaller.ComponentSelection).entered.connect(this, this.componentSelectionPageEntered);
}

Component.prototype.targetChooserClicked = function()
{
    var dir = QFileDialog.getExistingDirectory("", targetDirectoryPage.targetDirectory.text);
    targetDirectoryPage.targetDirectory.setText(dir);
}

Component.prototype.targetDirectoryChanged = function()
{
    var dir = targetDirectoryPage.targetDirectory.text;
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Existing installation detected and will be overwritten.</p>");
    }
    else if (installer.fileExists(dir)) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Installing in existing directory. It will be wiped on uninstallation.</p>");
    }
    else {
        targetDirectoryPage.warning.setText("");
    }
    installer.setValue("TargetDir", dir);
}

Component.prototype.componentSelectionPageEntered = function()
{
    var dir = installer.value("TargetDir");
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        installer.execute(dir + "/maintenancetool.exe", "--script=" + dir + "/scripts/auto_uninstall.qs");
    }
}
