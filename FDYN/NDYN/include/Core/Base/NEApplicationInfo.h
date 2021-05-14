#pragma once

//! NEApplictation is a base Application

/*
    Such a class is required to make Neutrino open scene files with double click on Mac
*/

class NECEXP_CORE NEApplicationInfo
{

public:
    NEApplicationInfo(std::shared_ptr<QCoreApplication> app, int& argc, char **argv);
    virtual ~NEApplicationInfo();
    QString passedFile() const;
    void setPassedFile(const QString &passedFile);

    bool serverMode() const;
    void setServerMode(int ServerMode);

    QString initPyFile() const;
    void setInitPyFile(const QString &initPyFile);

    int ServerPort() const;
    void setServerPort(int ServerPort);

    bool GuiMode() const;
    void setGuiMode(bool GuiMode);

    QString neSceneFile() const;
    void neSceneFile(const QString &sceneFile);

    bool runSim() const;
    void setRunSim( bool runSim );


    QCommandLineParser &parser();

    virtual void parseOptions();
    void setOptions();
protected:
    QString m_PassedFile;
    bool    m_ServerMode;
    int     m_ServerPort;
    bool    m_GuiMode;
    QString m_initPyFile;
    QString m_neSceneFile;

    bool    m_runSim;

    QCommandLineParser m_parser;

    QCommandLineOption* m_serverOption;
    QCommandLineOption* m_noServerOption;
    QCommandLineOption* m_serverPortOption;
    QCommandLineOption* m_noGuiOption;
    QCommandLineOption* m_sourceFileOption;
    QCommandLineOption* m_neFileOption;
    QCommandLineOption* m_runOption;
    QCommandLineOption* m_renderOption;

    std::shared_ptr<QCoreApplication> m_app;
};
