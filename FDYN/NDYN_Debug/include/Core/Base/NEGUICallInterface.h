#pragma once

class NENode;
class FilePath;

class ICallInterfaceFromBaseToGUI
{

public:
    ICallInterfaceFromBaseToGUI(){}
    virtual void updateInitFlagForNodeGraph(bool) {}
    virtual void restoreGUIState( QXmlStreamReader* ) {}
    virtual void updateNodeRenameForNodeGraph(const QString&, const QString&) {}
    virtual void updateNodeRenameToAllGUI(NENode*) {}
    virtual void updateInternalConnectionsTable() {}
    virtual QDebug& log()
    {
        static QString console_LogBuffer;
        static QDebug consoleLog = QDebug( &console_LogBuffer );
        return consoleLog;
    }
    virtual void endLog(NE::NELogType)
    {
    }
    virtual void EvalPyGUI(const QString&)
    {
    }
    virtual void updateGLWindowsFromBase() {}
    virtual void addCameraIntoContextMenus(NENode*) {}
    virtual void removeCameraFromContextMenus(NENode*) {}

    virtual void objectCreated(NENode*) {}
    virtual void objectRemoved(NENode*) {}

    virtual void selectObject(QObject*) {}
    virtual void updateNodeGraphEditor() {}
    virtual void updateNodeConnectionsTable() {}

    virtual void resizeGLWindows() {}

    virtual QStringList fileImportBrowser( FilePath& filePath,
                                           QString orgName = "",
                                           QString appName = "" ,
                                           QString settingsName = ""
            ) { return QStringList(); }
};
