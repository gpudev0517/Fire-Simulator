

function Component()
{
    // constructor
	    if (installer.value("os") === "win")
    {
		var programFiles = installer.environmentVariable("ProgramW6432");
		if (programFiles != "")
			installer.setValue("TargetDir", programFiles + "/FRI3D");
	}
}

Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
    } catch (e) {
        print(e);
    }
    if (installer.value("os") === "win")
    {
	  var registryVC2017x64 = installer.execute("reg", new Array("QUERY", "HKLM\\SOFTWARE\\WOW6432Node\\Microsoft\\VisualStudio\\14.0\\VC\\RunTimes\\x64", "/v", "Installed"))[0];
	  if (!registryVC2017x64) 
	  {
	      // VC2017 x86 redist not found: do something
		  component.addOperation("Execute", "@TargetDir@/vcredist_x64.exe", "/passive", "/norestart");

	  }
	  //component.addOperation("Execute", "@TargetDir@/AccessDatabaseEngine_X64.exe", "/passive", "/norestart");
      component.addOperation("CreateShortcut", "@TargetDir@/FRI3D.bat", "@StartMenuDir@/FRI3D.lnk");
      component.addOperation("CreateShortcut", "@TargetDir@/FRI3D.bat", "@DesktopDir@/FRI3D.lnk");
      component.addOperation("CreateShortcut", "@TargetDir@/uninstall.exe", "@StartMenuDir@/Uninstall.lnk");
      component.addOperation("RegisterFileType", "fri3d", "@TargetDir@\\FRI3D.bat" + " '%1'", "FRI3D Scene File", "text/plain", "@TargetDir@/FRI3D.exe"+","+0, "ProgId=FRI3D.fri3d");
      component.addOperation("Execute", "@TargetDir@\\Shortcut.exe", "/f:"+"@DesktopDir@\\FRI3D.lnk", "/a:e", "/I:"+"@TargetDir@\\FRI3D.exe,0");
      component.addOperation("Execute", "@TargetDir@\\Shortcut.exe", "/f:"+"@StartMenuDir@\\FRI3D.lnk", "/a:e", "/I:"+"@TargetDir@\\FRI3D.exe,0");
      component.addOperation("Delete", "@TargetDir@/Shortcut.exe");

    }
    else if (installer.value("os") === "x11")
    {
      component.addOperation("Execute", "xdg-mime", "install", "@TargetDir@/fri3d-mime.xml", "UNDOEXECUTE", "xdg-mime", "uninstall", "@TargetDir@/fri3d-mime.xml");
      component.addOperation("Execute", "mkdir", "-p", "@HomeDir@/.local/share/icons/hicolor/512x512/mimetypes", "UNDOEXECUTE", "rmdir", "@HomeDir@/.local/share/icons/hicolor/512x512/mimetypes");
      component.addOperation("Execute", "cp", "@TargetDir@/application-x-fri3d.png", "@HomeDir@/.local/share/icons/hicolor/512x512/mimetypes/application-x-fri3d.png", "UNDOEXECUTE", "rm", "@HomeDir@/.local/share/icons/hicolor/512x512/mimetypes/application-x-nescene.png");
     // component.addOperation("Execute", "xdg-icon-resource", "install", "--context mimetypes --size 512 @TargetDir@/application-x-fri3d.png application/x-fri3d", "UNDOEXECUTE", "xdg-icon-resource", "uninstall", "--context mimetypes --size 512 application/x-nescene");
      component.addOperation("CreateDesktopEntry", "fri3d.desktop", 
    "Type=Application\nName=FRI3D\nExec=@TargetDir@/fri3d %f\nPath=@TargetDir@\nIcon=@TargetDir@/fri3d.png\nTerminal=true\nCategories=Graphics\nMimeType=application/x-fri3d");
      component.addOperation("CreateShortcut", "@TargetDir@/fri3d.sh", "@DesktopDir@/FRI3D");
      component.addOperation("Mkdir", "@TargetDir@/lib/fonts");
      component.addOperation("Execute", "cp", "@TargetDir@/System/Fonts/consola.ttf", "@TargetDir@/lib/fonts", "UNDOEXECUTE", "rm", "@TargetDir@/lib/fonts/consola.ttf");
    //  component.addOperation("LineReplace", "@TargetDir@/qt.conf", "Prefix = ", "Prefix = @TargetDir@");
    }
    else if (installer.value("os") === "mac")
    {
        component.addOperation("Execute", "ln", "-s", "@TargetDir@/FRI3D.app", "@ApplicationsDir@/FRI3D", "UNDOEXECUTE", "rm", "@ApplicationsDir@/FRI3D");
      //component.addOperation("CreateShortcut", "@TargetDir@/FRI3D.app/Contents/MacOS/FRI3D.sh", "@ApplicationsDir@/FRI3D");
    }
}
