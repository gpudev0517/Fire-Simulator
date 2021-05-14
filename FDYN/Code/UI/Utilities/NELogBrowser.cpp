#include "Managers/NEGUIManager.h"
#include "Windows/Main/NEMainWindow.h"
#include "Core/Base/NEBase.h"
#include "Utilities/NELogBrowser.h"

#include <QMetaType>

NELogBrowser::NELogBrowser( QObject *parent ): QObject( parent )
{
    qRegisterMetaType<QtMsgType>("QtMsgType");
}


NELogBrowser::~NELogBrowser()
{
}


void NELogBrowser::outputMessage( QtMsgType type, const QString &msg )
{
    if( NEGUI.mainWindow() == NULL )
    {
        switch (type) {
        case QtDebugMsg:
            std::cerr << "Neutrino - Info: " << msg.toStdString() << std::endl;
            break;
        case QtWarningMsg:
            std::cerr << "Neutrino - Warning: " << msg.toStdString() << std::endl;
            break;

        case QtCriticalMsg:
            std::cerr << "Neutrino - Critical:" << msg.toStdString() << std::endl;
            break;

        case QtFatalMsg:
            std::cerr << "Neutrino - Fatal: " << msg.toStdString() << std::endl;
            break;
        }
    }
    else
    {
        switch (type) {
        case QtDebugMsg:
            Log() << "Neutrino - Info: " << msg; EndLog( NE::kLogInfo );
            break;
        case QtWarningMsg:
            Log() << "Neutrino - Warning: " << msg; EndLog( NE::kLogWarning );
            break;

        case QtCriticalMsg:
            Log() << "Neutrino - Critical: " << msg ; EndLog( NE::kLogError );
            break;

        case QtFatalMsg:
            Log() << "Neutrino - Fatal: " << msg ; EndLog( NE::kLogError );
            break;
        }
    }
}


