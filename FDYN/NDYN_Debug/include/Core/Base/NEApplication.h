#pragma once

class NEApplicationInfo;

//! NECoreApplictation is a QCoreApplication with no UI
/*!
    Such a class is required to make Neutrino open scene files with double click on Mac
*/

class NECEXP_CORE NEApplication: public QObject
{
    Q_OBJECT

public:
    NEApplication();
    NEApplication(int& argc, char **argv);
    virtual ~NEApplication();

    virtual void run() const;

    std::shared_ptr<QCoreApplication> instance()
    {
        return m_app;
    }

    bool    guiMode()       const;
    QString passedFile()    const;
    bool    serverMode()    const;
    QString initPyFile()    const;
    int     ServerPort()    const;
    QString neSceneFile()   const;
    bool    runSim()        const;

    int     serverPort()    const;

    virtual void parseOptions();
protected:

    std::shared_ptr<QCoreApplication>   m_app;
    std::shared_ptr<NEApplicationInfo>  m_info;
};
